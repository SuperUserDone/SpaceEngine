#pragma once
#include "tracy/Tracy.hpp"

#include <freetype/ftsizes.h>
#include <freetype/fttypes.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

#define PADDING 1

struct internal_font;

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

class glyph_builder {
public:
  render_text_location create(size_t key);

  void cleanup(render_text_location location) {
    // TODO
  }

  void set_font(internal_font *font) {
    m_font = font;
  }

private:
  internal_font *m_font;
};