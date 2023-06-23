#pragma once
#include "pyrolib/container/lru_cache.hh"
#include "renderer/render_batch.hh"
#include "renderer/text/glyph_builder.hh"
#include <freetype/freetype.h>

struct internal_font {
  FT_Face ft_face;
  void *font_face_data;

  render::batch batch;
  texture_data glyph_data;
  glm::uvec2 glyph_data_offset;
  size_t row_height;
  renderer_texture glyph_texture;

  bool tex_dirty;
  bool invalidate;
  size_t invalidation_count;

  pyro::container::hash_table<uint32_t, internal_size_info> size_lookup;
  pyro::container::lru_cache<size_t, render_text_location, glyph_builder> locations;
};