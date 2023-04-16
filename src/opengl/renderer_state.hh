#pragma once

#include "common/memory_arena.hh"
#include "common/memory_pool.hh"
struct internal_mesh {
  uint32_t vb;
  uint32_t ib;
  uint32_t vao;
};

struct renderer_state {
  mem_pool<internal_mesh> internal_mesh_data;
  mem_arena perm_data;
};

extern renderer_state* rstate;
