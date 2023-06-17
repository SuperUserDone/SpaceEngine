#pragma once

// This is used to get a handle to thread-local arena objects for temp scratch allocations.

#include "pyrolib/utils/api.hh"
#include <pyrolib/memory/arena.hh>

#define MAX_SCRATCH_STACKS 8

namespace pyro {
namespace memory {

struct scratch_arena {
  pyro::memory::arena &arena;
  size_t index;

  operator pyro::memory::arena &() {
    return arena;
  }
};

PYROAPI extern scratch_arena scratch_get();
PYROAPI extern void scratch_free(scratch_arena &scratch);

} // namespace memory
} // namespace pyro
