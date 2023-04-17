#pragma once

#include "data/asset_types.hh"
#include "glad/gl.h"
#include "tracy/Tracy.hpp"

static inline void update_texture(renderer_texture *r, texture_data *data) {
  ZoneScopedN("Update GPU Texture");
  glBindTexture(GL_TEXTURE_2D, r->index);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data->w, data->h, 0, GL_RGB, GL_UNSIGNED_BYTE, data->data);
}

static inline renderer_texture create_texture(texture_data *data) {
  ZoneScopedN("Create GPU Texture");
  renderer_texture tex;

  glGenTextures(1, &tex.index);
  update_texture(&tex, data);
  return tex;
}

static inline void delete_texture(renderer_texture tex) {
  ZoneScopedN("Delete GPU Texture");
  glDeleteTextures(1, &tex.index);
}
