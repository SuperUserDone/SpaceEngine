#pragma once

#include "data/asset_types.hh"
#include "glad/gl.h"
#include <corecrt_wstring.h>

static inline void update_texture(renderer_texture *r, texture_data *data) {
  glBindTexture(GL_TEXTURE_2D, r->index);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data->w, data->h, 0, GL_RGB, GL_UNSIGNED_BYTE, data->data);
}

static inline renderer_texture create_texture(texture_data *data) {
  renderer_texture tex;

  glGenTextures(1, &tex.index);
  update_texture(&tex, data);
  return tex;
}

static inline void delete_texture(renderer_texture tex) {
  glDeleteTextures(1, &tex.index);
}
