#pragma once

#include "memory/memory_pool.hh"
#include "pyrolib/memory/arena.hh"

struct internal_mesh {
  uint32_t vb;
  uint32_t ib;
  uint32_t vao;
};

struct renderer_state {
  mem_pool<internal_mesh> internal_mesh_data;
  pyro::memory::arena perm_data;
};

extern renderer_state *rstate;
