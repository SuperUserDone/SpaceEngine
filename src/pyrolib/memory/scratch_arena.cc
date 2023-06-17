#include "scratch_arena.hh"
#include "common/debug.hh"

namespace pyro {
namespace memory {
thread_local static pyro::memory::arena arenas[MAX_SCRATCH_STACKS];
thread_local static bool init = false;
thread_local static size_t index = 0;

scratch_arena scratch_get() {
  // Lazy-init the arenas
  if (!init) {
    for (size_t i = 0; i < MAX_SCRATCH_STACKS; i++) {
      arenas[i].lt_init(4096 * 2048); // 2048 pages
    }
    init = true;
  }

  // Ensure an arena exists with the required size
  SPACE_ASSERT(index < MAX_SCRATCH_STACKS,
               "Attempt to get more handles to scratch arenas than allowed (%d)",
               MAX_SCRATCH_STACKS);
  scratch_arena a = {arenas[index], index};
  index++;

  return a;
}

void scratch_free(scratch_arena &arena) {
  SPACE_ASSERT(index > 0, "Tried to free an scratch arena without one being allocated!");

  index--;
  SPACE_ASSERT(index == arena.index,
               "Attempting to free a scratch buffer allocated before another index = %llu and "
               "arena.index = %llu",
               index,
               arena.index);

  // Clear the arena for the next person.
  arena.arena.clear();
}

} // namespace memory
} // namespace pyro