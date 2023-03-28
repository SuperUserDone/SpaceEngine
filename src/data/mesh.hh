#pragma once

#include <stdint.h>

struct mesh_vertex {
  float x;
  float y;
};

struct mesh {
  mesh_vertex *verticies;
  uint32_t *indicies;

  size_t vertex_count;
  size_t index_count;
};