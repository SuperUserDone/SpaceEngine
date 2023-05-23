#include "memory_scratch_arena.hh"
#include "common/debug.hh"
#include "memory_arena.hh"

thread_local static mem_arena arenas[MAX_SCRATCH_STACKS];
thread_local static bool init = false;
thread_local static size_t index = 0;

mem_scratch_arena arena_scratch_get() {
  // Lazy-init the arenas
  if (!init) {
    for (size_t i = 0; i < MAX_SCRATCH_STACKS; i++) {
      arenas[i] = arena_create();
    }
    init = true;
  }

  // Ensure an arena exists with the required size
  SPACE_ASSERT(index < MAX_SCRATCH_STACKS,
               "Attempt to get more handles to scratch arenas than allowed (%d)",
               MAX_SCRATCH_STACKS);
  mem_scratch_arena a = {arenas[index], index};
  index++;

  return a;
}

void arena_scratch_free(mem_scratch_arena &arena) {
  SPACE_ASSERT(index > 0, "Tried to free an scratch arena without one being allocated!");

  index--;
  SPACE_ASSERT(index == arena.index,
               "Attempting to free a scratch buffer allocated before another index = %llu and "
               "arena.index = %llu",
               index,
               arena.index);

  // Clear the arena for the next person.
  arena_pop_to(arena, arena.arena.base);
}
