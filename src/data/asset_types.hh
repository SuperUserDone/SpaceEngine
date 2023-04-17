#pragma once

// Defines some structs to aid with management of assets and renderer data.
// structs starting with renderer_* should be treated as opaque, but I have decided to not make them
// opaque pointers to avoid a _possible_ cache miss when looking up the actual values we need.

#include <glm/glm.hpp>
#include <stdint.h>

#define PIPELINE_MAX_UNIFORM 64 

struct renderer_texture {
  uint32_t index;
};

struct texture_data {
  void *data;
  uint16_t w;
  uint16_t h;
};

struct renderer_mesh {
  uint32_t index;
  uint32_t triangle_vertex_count;
};

struct vertex {
  glm::vec2 pos;
  glm::vec2 uv;
};

struct mesh_data {
  vertex *verticies;
  uint32_t *indicies;

  size_t vertex_count;
  size_t index_count;
};

struct pipeline_data {
  char* fragment_shader;
  char* vertex_shader;

  const char** uniform_names;
  size_t uniform_count;
};

struct renderer_pipeline {
  uint32_t index;

  size_t *uniform_indicies;
};
