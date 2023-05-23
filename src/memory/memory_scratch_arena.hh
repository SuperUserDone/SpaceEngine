#pragma once

// This is used to get a handle to thread-local arena objects for temp scratch allocations.

#include "memory_arena.hh"

#define MAX_SCRATCH_STACKS 8

struct mem_scratch_arena {
  mem_arena &arena;
  size_t index;

  operator mem_arena &() {
    return arena;
  }
};

APIFUNC mem_scratch_arena arena_scratch_get();
APIFUNC void arena_scratch_free(mem_scratch_arena &scratch);
