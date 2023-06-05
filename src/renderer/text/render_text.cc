#include "render_text.hh"
#include "assetmanager/assetmanager.hh"
#include "common/debug.hh"
#include "common/hash_table.hh"
#include "common/lru_cache.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/glm_exts.hh"
#include "memory/memory_pool.hh"
#include "pyrolib/container/arena_vector.hh"
#include "renderer/render_batch.hh"
#include "renderer/text/render_text.hh"
#include "tracy/Tracy.hpp"

#include <freetype/freetype.h>
#include <freetype/ftsizes.h>
#include <freetype/fttypes.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <stdint.h>
#include <vcruntime_string.h>

#define PADDING 1

struct render_text_location {
  glm::vec2 uva;
  glm::vec2 uvb;
  glm::vec2 bearing;
  glm::vec2 isize;
};

struct internal_size_info {
  FT_Size size;
  size_t line_height;

  hb_font_t *hb_font;
  hb_buffer_t *hb_buffer;
};

struct internal_font {
  FT_Face ft_face;
  void *font_face_data;

  render_batch batch;
  texture_data glyph_data;
  glm::uvec2 glyph_data_offset;
  size_t row_height;
  renderer_texture glyph_texture;

  bool tex_dirty;
  bool invalidate;
  size_t invalidation_count;

  pyro::container::arena_vector<internal_size_info> sizes;
  hash_table<uint32_t, internal_size_info> size_lookup;
  lru_cache<size_t, render_text_location> locations;
};

struct render_text_state {
  FT_Library ft_lib;

  mem_pool<internal_font> fonts;
};

render_text_location create_glyph(size_t key, void *userdata) {
  ZoneScopedN("Create Text Glyph");
  const int padding = PADDING;
  internal_font *state = (internal_font *)userdata;

  uint32_t font_size = (key >> 32);
  uint32_t codepoint = key & 0xffffffff;

  internal_size_info *size_info = hash_table_search(state->size_lookup, font_size);

  FT_Activate_Size(size_info->size);

  render_text_location location;

  FT_Int32 flags = FT_LOAD_DEFAULT;

  FT_Load_Glyph(state->ft_face, codepoint, flags);

  FT_GlyphSlot slot = state->ft_face->glyph;
  FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

  FT_Bitmap ftBitmap = slot->bitmap;

  glm::ivec2 size{ftBitmap.width, ftBitmap.rows};

  if (state->glyph_data_offset.x + size.x > state->glyph_data.w) {
    state->glyph_data_offset.x = padding;
    state->glyph_data_offset.y += state->row_height;
    state->row_height = padding;
  }

  if (size.y + state->glyph_data_offset.y >= state->glyph_data.h) {
    state->invalidate = true;
  } else {
    for (int i = 0; i < size.y; i++) {
      size_t x_offset = state->glyph_data_offset.x;
      size_t y_offset = state->glyph_data_offset.y;
      size_t w = state->glyph_data.w;
      memcpy((uint8_t *)state->glyph_data.data + x_offset + ((i + y_offset) * w),
             (uint8_t *)ftBitmap.buffer + i * size.x,
             size.x);
    }
  }

  location.uva = glm::vec2(state->glyph_data_offset) / (float)state->glyph_data.h;
  location.uvb = location.uva + (glm::vec2(size) / (float)state->glyph_data.h);
  location.isize = size;
  location.bearing = glm::vec2(slot->bitmap_left, -slot->bitmap_top);

  state->glyph_data_offset.x += size.x + padding;
  state->row_height = std::max(state->row_height, (size_t)size.y + padding);

  state->tex_dirty = true;
  return location;
}

void update_gpu_texture(app_state *state, internal_font *font) {
  if (font->tex_dirty) {
    state->api.renderer.update_texture(&font->glyph_texture, &font->glyph_data);
    font->tex_dirty = false;
  }
}

void delete_glyph(render_text_location value, void *userdata) {
  // app_state *state = (app_state *)userdata;
  // TODO
}

void init_ft(app_state *state) {
  render_text_state *info = state->render_text_state;

  FT_Error ft_error;

  SPACE_ASSERT(!(ft_error = FT_Init_FreeType(&info->ft_lib)), "Could not init freetype!");
}

void quit_ft(app_state *state) {
  render_text_state *info = state->render_text_state;
  FT_Done_FreeType(info->ft_lib);
}

APIFUNC extern renderer_font render_font_create(app_state *state, font_data *data) {
  render_text_state *info = state->render_text_state;
  internal_font *i_font = pool_alloc(state->render_text_state->fonts);
  i_font->invalidate = false;
  i_font->invalidation_count = 0;

  i_font->font_face_data = new uint8_t[data->file_len];
  memcpy(i_font->font_face_data, data->file_data, data->file_len);

  SPACE_ASSERT(!FT_New_Memory_Face(info->ft_lib,
                                   (FT_Byte *)i_font->font_face_data,
                                   data->file_len,
                                   0,
                                   &i_font->ft_face),
               "Could not load face!");
  i_font->locations = lru_cache_create(state->permanent_arena,
                                       FONT_MAX_GLYPH_COUNT,
                                       create_glyph,
                                       delete_glyph,
                                       i_font);
  i_font->size_lookup = hash_table_create<uint32_t, internal_size_info>(state->permanent_arena);
  i_font->sizes.lt_init(FONT_SIZE_MAX - FONT_SIZE_MIN + 1);

  size_t texture_size =
      std::min(state->api.renderer.get_max_texture_size(), (size_t)FONT_MIN_GLYPH_ATLAS_SIZE);
  i_font->glyph_data.data = calloc(sizeof(uint8_t), texture_size * texture_size);
  i_font->glyph_data.h = texture_size;
  i_font->glyph_data.w = texture_size;
  i_font->glyph_data.format = TEX_FORMAT_R;
  i_font->glyph_data.downscale = TEX_FILTER_LINEAR;
  i_font->glyph_data.upscale = TEX_FILTER_LINEAR;
  i_font->glyph_data.edge = TEX_EDGE_CLAMP;

  i_font->glyph_texture = state->api.renderer.create_texture(&i_font->glyph_data);

  i_font->glyph_data_offset = {PADDING, PADDING};
  i_font->row_height = 0;

  i_font->tex_dirty = false;

  i_font->batch = render_batch_create(state);

  renderer_font font;
  font.index = (size_t)i_font;
  return font;
}

APIFUNC render_font_info render_font_get_info(app_state *state, renderer_font font) {
  internal_font *i_font = (internal_font *)font.index;
  render_font_info info;

  info.atlas_size = {i_font->glyph_data.w, i_font->glyph_data.h};
  info.ht_entries = i_font->locations.index.nentries;
  info.texture = i_font->glyph_texture;
  info.cache_entries = i_font->locations.nentries;

  return info;
}

void render_text_init(app_state *state) {
  state->render_text_state = state->permanent_arena.push<render_text_state>();
  state->render_text_state->fonts = pool_create<internal_font>(64);

  init_ft(state);
}

void render_text_quit(app_state *state) {
  quit_ft(state);
}

void queue_rect(render_batch &batch, glm::vec2 pos, glm::vec2 size, glm::vec2 uva, glm::vec2 uvb) {
  rect r = {pos, size, uva, uvb};
  render_batch_add_rect(batch, r);
}

void render_text(app_state *state,
                 renderer_font font,
                 uint32_t size,
                 glm::vec2 pos,
                 const char *text) {
  ZoneScopedN("Queue Draw Text");

  SPACE_ASSERT(size >= FONT_SIZE_MIN && size <= FONT_SIZE_MAX,
               "Font size must be between %d and %d!",
               FONT_SIZE_MIN,
               FONT_SIZE_MAX);

  float dpi = state->window_area.dpi_scaling;

  uint32_t scaled_size = size * dpi;

  internal_font *i_font = (internal_font *)font.index;
  internal_size_info *i_size = hash_table_search(i_font->size_lookup, (uint32_t)scaled_size);

  if (!i_size) {
    i_font->sizes.push_back({});
    i_size = &i_font->sizes[i_font->sizes.size() - 1];
    hash_table_insert(i_font->size_lookup, scaled_size, i_size);

    FT_New_Size(i_font->ft_face, &i_size->size);
    FT_Activate_Size(i_size->size);
    FT_Set_Pixel_Sizes(i_font->ft_face, 0, scaled_size);
    i_size->hb_font = hb_ft_font_create(i_font->ft_face, nullptr);
    i_size->hb_buffer = hb_buffer_create();

    i_size->line_height = i_size->size->metrics.ascender;
  }

  glm::vec2 current_pos = pos * dpi;
  FT_Activate_Size(i_size->size);

  {
    ZoneScopedN("Reset Buffer");
    hb_buffer_reset(i_size->hb_buffer);
  }

  {
    ZoneScopedN("Add UTF8 Text to buffer");
    hb_buffer_add_utf8(i_size->hb_buffer, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(i_size->hb_buffer);
  }

  {
    ZoneScopedN("Shape");
    hb_shape(i_size->hb_font, i_size->hb_buffer, NULL, 0);
  }

  {
    ZoneScopedN("Process Glyphs");
    uint32_t len = hb_buffer_get_length(i_size->hb_buffer);
    hb_glyph_info_t *buffer_info = hb_buffer_get_glyph_infos(i_size->hb_buffer, NULL);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(i_size->hb_buffer, NULL);

    for (size_t i = 0; i < len; i++) {
      uint32_t cp = buffer_info[i].codepoint;
      render_text_location glyph;

      {
        ZoneScopedN("Lookup Glyph");
        uint64_t key = cp | ((uint64_t)scaled_size << 32);
        glyph = *lru_cache_find(i_font->locations, key);
      }

      glm::vec2 rect_pos = current_pos +
                           (glm::vec2((float)pos[i].x_offset, -pos[i].y_offset) / 64.f) +
                           glyph.bearing;
      glm::vec2 rect_size = glyph.isize;

      queue_rect(i_font->batch, rect_pos / dpi, rect_size / dpi, glyph.uva, glyph.uvb);
      current_pos += glm::vec2(pos[i].x_advance, -pos[i].y_advance) / 64.f;
    }
  }
}

void render_font_reset(app_state *state, renderer_font font) {
  internal_font *i_font = (internal_font *)font.index;
  render_batch_reset(i_font->batch);
  if (i_font->invalidate) {
    lru_cache_invalidate(i_font->locations);
    i_font->glyph_data_offset = {PADDING, PADDING};
    i_font->row_height = 0;
    i_font->invalidate = false;
    i_font->invalidation_count++;

    if (i_font->invalidation_count >= FONT_ATLAS_RESIZE_THRES) {
      free(i_font->glyph_data.data);

      size_t texture_size = std::min(
          std::min(state->api.renderer.get_max_texture_size(), (size_t)FONT_MAX_GLYPH_ATLAS_SIZE),
          (size_t)i_font->glyph_data.w * 2);
      i_font->glyph_data.data = calloc(sizeof(uint8_t), texture_size * texture_size);

      i_font->glyph_data.h = texture_size;
      i_font->glyph_data.w = texture_size;

      i_font->invalidation_count = 0;
      return;
    }

    memset(i_font->glyph_data.data, 0x00, i_font->glyph_data.w * i_font->glyph_data.h);
  }
}

void render_font_finish(app_state *state, renderer_font font) {
  internal_font *i_font = (internal_font *)font.index;
  update_gpu_texture(state, i_font);

  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;

  glm::mat4 mvp = glm::ortho(0.f, area.x, area.y, 0.f, -1.f, 1.f);

  renderer_pipeline p = asset_pipeline_get_render(state, "text");

  pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);
  pipeline_settings_set_uniform(settings, 0, mvp);
  pipeline_settings_set_uniform(settings, 1, i_font->glyph_texture);

  render_batch_render(state, i_font->batch, p, settings);
}

void render_font_delete(app_state *state, renderer_font font) {
  internal_font *i_font = (internal_font *)font.index;

  for (auto &sz : i_font->sizes) {
    hb_buffer_destroy(sz.hb_buffer);
    hb_font_destroy(sz.hb_font);
    FT_Done_Size(sz.size);
  }

  FT_Done_Face(i_font->ft_face);
}