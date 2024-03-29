#pragma once

// Defines some structs to aid with management of assets and renderer data.
// structs starting with renderer_* should be treated as opaque, but I have decided to not make them
// opaque pointers to avoid a _possible_ cache miss when looking up the actual values we need. Might
// not be neccacary though, I have not benchmarked it

#include "pyrolib/math/math.hh"
#include <glm/glm.hpp>
#include <pyrolib/container/array.hh>
#include <stdint.h>

enum texture_formats {
  TEX_FORMAT_R,
  TEX_FORMAT_RG,
  TEX_FORMAT_RGB,
  TEX_FORMAT_RGBA,
  TEX_FORMAT_RGBA_F16,
  TEX_FORMAT_SRGB,
  TEX_FORMAT_SRGBA
};

enum texture_filter {
  TEX_FILTER_LINEAR,
  TEX_FILTER_NEAREST,
};

enum texture_edge {
  TEX_EDGE_CLAMP,
  TEX_EDGE_REPEAT
};

struct renderer_texture {
  uint32_t index;
};

struct texture_data {
  void *data;

  texture_formats format;
  texture_filter upscale;
  texture_filter downscale;

  texture_edge edge;

  uint16_t w;
  uint16_t h;
};

struct renderer_mesh {
  size_t index;
  uint32_t triangle_vertex_count;
};

struct mesh_data {
  pyro::container::array<pyro::vertex> verticies;
  pyro::container::array<uint16_t> indicies;
};

struct pipeline_data {
  char *fragment_shader;
  char *vertex_shader;

  const char **uniform_names;
  size_t uniform_count;
};

struct renderer_pipeline {
  uint32_t index;
  pyro::container::array<size_t> uniform_indicies;
};

struct framebuffer_data {
  renderer_texture color_attachment;
};

struct renderer_framebuffer {
  uint32_t index;
};

struct renderer_font {
  size_t index;
};

struct font_data {
  void *file_data;
  size_t file_len;
};
