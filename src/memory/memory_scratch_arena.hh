#pragma once

// This is used to get a handle to thread-local arena objects for temp scratch allocations.

#include <pyrolib/memory/arena.hh>

#define MAX_SCRATCH_STACKS 8

struct mem_scratch_arena {
  pyro::memory::arena &arena;
  size_t index;

  operator pyro::memory::arena &() {
    return arena;
  }
};

APIFUNC extern mem_scratch_arena arena_scratch_get();
APIFUNC extern void arena_scratch_free(mem_scratch_arena &scratch);
