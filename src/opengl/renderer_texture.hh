#pragma once

#include "data/asset_types.hh"
#include "glad/gl.h"
#include "tracy/Tracy.hpp"

static inline GLenum tex_type_to_internal_type(texture_formats f) {
  switch (f) {
  case TEX_FORMAT_R:
    return GL_R8;
  case TEX_FORMAT_RG:
    return GL_RG8;
  case TEX_FORMAT_RGB:
    return GL_RGB8;
  case TEX_FORMAT_RGBA:
    return GL_RGBA8;
  case TEX_FORMAT_SRGB:
    return GL_SRGB8;
  case TEX_FORMAT_SRGBA:
    return GL_SRGB8_ALPHA8;
  case TEX_FORMAT_RGBA_F16:
    return GL_RGBA16F;
    break;
  }
}

static inline GLenum tex_filter_to_gl(texture_filter filter) {
  switch (filter) {
  case TEX_FILTER_LINEAR:
    return GL_LINEAR;
  case TEX_FILTER_NEAREST:
    return GL_NEAREST;
    break;
  }
}

static inline GLenum tex_type_to_external(texture_formats f) {
  switch (f) {
  case TEX_FORMAT_R:
    return GL_RED;
  case TEX_FORMAT_RG:
    return GL_RG;
  case TEX_FORMAT_RGB:
  case TEX_FORMAT_SRGB:
    return GL_RGB;
  case TEX_FORMAT_RGBA:
  case TEX_FORMAT_RGBA_F16:
  case TEX_FORMAT_SRGBA:
    return GL_RGBA;
  }
}

static inline GLenum tex_edge_to_gl(texture_edge e) {
  switch (e) {
  case TEX_EDGE_CLAMP:
    return GL_CLAMP_TO_EDGE;
  case TEX_EDGE_REPEAT:
    return GL_REPEAT;
    break;
  }
}

static inline void update_texture(renderer_texture *r, texture_data *data) {
  ZoneScopedN("Update GPU Texture");
  glBindTexture(GL_TEXTURE_2D, r->index);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_edge_to_gl(data->edge));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_edge_to_gl(data->edge));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_filter_to_gl(data->downscale));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_filter_to_gl(data->upscale));
  glTexImage2D(GL_TEXTURE_2D,
               0,
               tex_type_to_internal_type(data->format),
               data->w,
               data->h,
               0,
               tex_type_to_external(data->format),
               GL_UNSIGNED_BYTE,
               data->data);
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
