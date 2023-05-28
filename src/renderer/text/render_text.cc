#include "render_text.hh"
#include "assetmanager/assetmanager.hh"
#include "common/lru_cache.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "memory/memory_arena.hh"
#include "memory/memory_arena_typed.hh"
#include "renderer/text/render_text.hh"
#include "tracy/Tracy.hpp"

#include <freetype/freetype.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <stdint.h>

#define PADDING 1

struct render_text_location {
  glm::vec2 uva;
  glm::vec2 uvb;
  glm::vec2 bearing;
  glm::ivec2 isize;
};

struct internal_info {
  FT_Library ft_lib;
  FT_Face ft_face;

  hb_font_t *hb_font;
  hb_buffer_t *hb_buffer;
};

struct render_text_state {
  mesh_data rect_data;
  renderer_mesh rect_mesh;

  bool tex_dirty;

  texture_data glyph_data;
  glm::uvec2 glyph_data_offset;
  size_t row_height;
  renderer_texture glyph_texture;

  mem_arena_typed<vertex> verticies_arena;
  mem_arena_typed<uint32_t> indicies_arena;

  lru_cache<size_t, render_text_location> locations;

  internal_info info;

  size_t line_height;
};

render_text_location create_glyph(size_t key, void *userdata) {
  ZoneScopedN("Create Text Glyph");
  const int padding = PADDING;
  app_state *state = (app_state *)userdata;
  internal_info &info = state->render_text_state->info;

  render_text_state *rs = state->render_text_state;

  render_text_location location;

  FT_Int32 flags = FT_LOAD_DEFAULT;

  FT_Load_Glyph(info.ft_face, key, flags);

  FT_GlyphSlot slot = info.ft_face->glyph;
  FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

  FT_Bitmap ftBitmap = slot->bitmap;

  glm::ivec2 size{ftBitmap.width, ftBitmap.rows};

  if (rs->glyph_data_offset.x + size.x > rs->glyph_data.w) {
    rs->glyph_data_offset.x = padding;
    rs->glyph_data_offset.y += rs->row_height;
    rs->row_height = padding;
  }

  for (int i = 0; i < size.y; i++) {
    size_t x_offset = rs->glyph_data_offset.x;
    size_t y_offset = rs->glyph_data_offset.y;
    size_t w = rs->glyph_data.w;
    memcpy((uint8_t *)rs->glyph_data.data + x_offset + ((i + y_offset) * w),
           (uint8_t *)ftBitmap.buffer + i * size.x,
           size.x);
  }

  location.uva = glm::vec2(rs->glyph_data_offset) / (float)rs->glyph_data.h;
  location.uvb = location.uva + (glm::vec2(size) / (float)rs->glyph_data.h);
  location.isize = size;
  location.bearing = glm::vec2(slot->bitmap_left, -slot->bitmap_top);

  rs->glyph_data_offset.x += size.x + padding;
  rs->row_height = std::max(rs->row_height, (size_t)size.y + padding);

  state->render_text_state->tex_dirty = true;
  return location;
}

void update_gpu_texture(app_state *state) {
  if (state->render_text_state->tex_dirty) {
    state->api.renderer.update_texture(&state->render_text_state->glyph_texture,
                                       &state->render_text_state->glyph_data);
    state->render_text_state->tex_dirty = false;
  }
}

void update_gpu_mesh(app_state *state) {
  state->api.renderer.update_mesh(&state->render_text_state->rect_mesh,
                                  &state->render_text_state->rect_data);
}

void delete_glyph(render_text_location value, void *userdata) {
  app_state *state = (app_state *)userdata;
}

void init_ft(app_state *state) {
  internal_info &info = state->render_text_state->info;

  FT_Error ft_error;

  if ((ft_error = FT_Init_FreeType(&info.ft_lib)))
    abort();
  if ((ft_error = FT_New_Face(info.ft_lib, "data/fonts/Roboto-Regular.ttf", 0, &info.ft_face)))
    abort();
  if ((ft_error = FT_Set_Pixel_Sizes(info.ft_face, 0, 48)))
    abort();

  state->render_text_state->line_height = info.ft_face->height;
}

void init_hb(app_state *state) {
  internal_info &info = state->render_text_state->info;
  info.hb_font = hb_ft_font_create(info.ft_face, nullptr);
  info.hb_buffer = hb_buffer_create();
}

void quit_ft(app_state *state) {
  internal_info &info = state->render_text_state->info;
  FT_Done_Face(info.ft_face);
  FT_Done_FreeType(info.ft_lib);
}
void quit_hb(app_state *state) {
  internal_info &info = state->render_text_state->info;
  hb_font_destroy(info.hb_font);
  hb_buffer_destroy(info.hb_buffer);
}

void render_text_init(app_state *state) {
  state->render_text_state = arena_push_struct(state->permanent_arena, render_text_state);

  state->render_text_state->verticies_arena = arena_typed_create<vertex>(65536 * 4);
  state->render_text_state->indicies_arena = arena_typed_create<uint32_t>(65536 * 6);

  state->render_text_state->rect_data.verticies =
      (vertex *)state->render_text_state->verticies_arena.arena.base;

  state->render_text_state->rect_data.indicies =
      (uint32_t *)state->render_text_state->indicies_arena.arena.base;

  state->render_text_state->locations =
      lru_cache_create(state->permanent_arena, 65536, create_glyph, delete_glyph, state);

  size_t texture_size = std::min(state->api.renderer.get_max_texture_size(), 4096ull);
  state->render_text_state->glyph_data.data =
      arena_push_zero(state->permanent_arena, texture_size * texture_size);
  state->render_text_state->glyph_data.h = texture_size;
  state->render_text_state->glyph_data.w = texture_size;
  state->render_text_state->glyph_data.format = TEX_FORMAT_R;
  state->render_text_state->glyph_data.downscale = TEX_FILTER_LINEAR;
  state->render_text_state->glyph_data.upscale = TEX_FILTER_LINEAR;
  state->render_text_state->glyph_data.edge = TEX_EDGE_CLAMP;

  state->render_text_state->glyph_texture =
      state->api.renderer.create_texture(&state->render_text_state->glyph_data);

  state->render_text_state->glyph_data_offset = {PADDING, PADDING};
  state->render_text_state->row_height = 0;

  state->render_text_state->rect_data.vertex_count = 0;
  state->render_text_state->rect_data.index_count = 0;

  state->render_text_state->rect_mesh =
      state->api.renderer.create_mesh(&state->render_text_state->rect_data);

  state->render_text_state->tex_dirty = false;

  init_ft(state);
  init_hb(state);
}

void render_text_quit(app_state *state) {
  quit_hb(state);
  quit_ft(state);

  arena_typed_free(state->render_text_state->indicies_arena);
  arena_typed_free(state->render_text_state->verticies_arena);
}

void render_text_newframe(app_state *state) {
  arena_typed_clear(state->render_text_state->indicies_arena);
  arena_typed_clear(state->render_text_state->verticies_arena);

  state->render_text_state->rect_data.vertex_count = 0;
  state->render_text_state->rect_data.index_count = 0;
}

void queue_rect(render_text_state *state,
                glm::vec2 pos,
                glm::vec2 size,
                glm::vec2 uva,
                glm::vec2 uvb) {
  *arena_typed_push(state->verticies_arena) = vertex{pos, uva};
  *arena_typed_push(state->verticies_arena) =
      vertex{glm::vec2(pos.x, pos.y + size.y), glm::vec2(uva.x, uvb.y)};
  *arena_typed_push(state->verticies_arena) =
      vertex{glm::vec2(pos.x + size.x, pos.y + size.y), uvb};
  *arena_typed_push(state->verticies_arena) =
      vertex{glm::vec2(pos.x + size.x, pos.y), glm::vec2{uvb.x, uva.y}};

  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 0;
  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 1;
  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 2;
  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 0;
  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 2;
  *arena_typed_push(state->indicies_arena) = state->rect_data.vertex_count + 3;

  state->rect_data.index_count += 6;
  state->rect_data.vertex_count += 4;
}

void render_text_queue(app_state *state, int x, int y, const char *text) {
  ZoneScopedN("Queue Draw Text");

  internal_info &info = state->render_text_state->info;
  glm::vec2 current_pos = {x, y};

  {
    ZoneScopedN("Reset Buffer");
    hb_buffer_reset(info.hb_buffer);
  }

  {
    ZoneScopedN("Add UTF8 Text to buffer");
    hb_buffer_add_utf8(info.hb_buffer, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(info.hb_buffer);
  }

  {
    ZoneScopedN("Shape");
    hb_shape(info.hb_font, info.hb_buffer, NULL, 0);
  }

  {
    ZoneScopedN("Process Glyphs");
    uint32_t len = hb_buffer_get_length(info.hb_buffer);
    hb_glyph_info_t *buffer_info = hb_buffer_get_glyph_infos(info.hb_buffer, NULL);
    hb_glyph_position_t *pos = hb_buffer_get_glyph_positions(info.hb_buffer, NULL);

    for (size_t i = 0; i < len; i++) {
      size_t cp = buffer_info[i].codepoint;
      render_text_location glyph;

      {
        ZoneScopedN("Lookup Glyph");
        glyph = *lru_cache_find(state->render_text_state->locations, cp);
      }

      glm::vec2 rect_pos =
          current_pos +
          (glm::vec2((float)pos[i].x_offset,
                     (float)state->render_text_state->line_height - pos[i].y_offset) /
           64.f) +
          glyph.bearing;
      glm::vec2 rect_size = glyph.isize;

      queue_rect(state->render_text_state, rect_pos, rect_size, glyph.uva, glyph.uvb);
      current_pos += glm::vec2(pos[i].x_advance, -pos[i].y_advance) / 64.f;
    }
  }
}

void render_text_finishframe(app_state *state) {
  update_gpu_texture(state);
  update_gpu_mesh(state);

  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  // area /= state->window_area.dpi_scaling;

  glm::mat4 mvp = glm::ortho(0.f, area.x, area.y, 0.f, -1.f, 1.f);

  renderer_mesh *mp = &state->render_text_state->rect_mesh;

  renderer_pipeline p = asset_pipeline_get_render(state, "text");

  pipeline_settings settings = pipeline_settings_create(p, state->frame_arena);
  pipeline_settings_set_uniform(settings, 0, mvp);
  pipeline_settings_set_uniform(settings, 1, state->render_text_state->glyph_texture);

  pipeline_settings *pp = &settings;
  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}