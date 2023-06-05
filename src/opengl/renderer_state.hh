#pragma once

#include "pyrolib/memory/arena.hh"
#include "pyrolib/memory/pool.hh"

struct internal_mesh {
  uint32_t vb;
  uint32_t ib;
  uint32_t vao;
};

struct renderer_state {
  pyro::memory::pool<internal_mesh> internal_mesh_data;
  pyro::memory::arena perm_data;
};

extern renderer_state *rstate;
