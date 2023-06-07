#include "glyph_builder.hh"
#include "internal_font.hh"

render_text_location glyph_builder::create(size_t key) {
  ZoneScopedN("Create Text Glyph");
  const int padding = PADDING;
  uint32_t font_size = (key >> 32);
  uint32_t codepoint = key & 0xffffffff;

  auto a = m_font->size_lookup.find(font_size);
  internal_size_info &size_info = a->val;

  FT_Activate_Size(size_info.size);

  render_text_location location;

  FT_Int32 flags = FT_LOAD_DEFAULT;

  FT_Load_Glyph(m_font->ft_face, codepoint, flags);

  FT_GlyphSlot slot = m_font->ft_face->glyph;
  FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

  FT_Bitmap ftBitmap = slot->bitmap;

  glm::ivec2 size{ftBitmap.width, ftBitmap.rows};

  if (m_font->glyph_data_offset.x + size.x > m_font->glyph_data.w) {
    m_font->glyph_data_offset.x = padding;
    m_font->glyph_data_offset.y += m_font->row_height;
    m_font->row_height = padding;
  }

  if (size.y + m_font->glyph_data_offset.y >= m_font->glyph_data.h) {
    m_font->invalidate = true;
  } else {
    for (int i = 0; i < size.y; i++) {
      size_t x_offset = m_font->glyph_data_offset.x;
      size_t y_offset = m_font->glyph_data_offset.y;
      size_t w = m_font->glyph_data.w;
      memcpy((uint8_t *)m_font->glyph_data.data + x_offset + ((i + y_offset) * w),
             (uint8_t *)ftBitmap.buffer + i * size.x,
             size.x);
    }
  }

  location.uva = glm::vec2(m_font->glyph_data_offset) / (float)m_font->glyph_data.h;
  location.uvb = location.uva + (glm::vec2(size) / (float)m_font->glyph_data.h);
  location.isize = size;
  location.bearing = glm::vec2(slot->bitmap_left, -slot->bitmap_top);

  m_font->glyph_data_offset.x += size.x + padding;
  m_font->row_height = std::max(m_font->row_height, (size_t)size.y + padding);

  m_font->tex_dirty = true;
  return location;
}