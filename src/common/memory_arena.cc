#include "memory_arena.hh"

#ifdef _WIN32

#include <windows.h>

static size_t page_size = 0x0;
static const size_t max_arena_size_default = 0x40000000; // 1 Gigibyte

mem_arena arena_create(size_t max_size) {
  mem_arena a;

  if (max_size == 0)
    max_size = max_arena_size_default;

  // Base alignment should be 1
  a.alignment = 1;
  a.base = nullptr;
  a.size = 0;
  a.grow_semaphore = false;

  // We need to keep track of the system page size so lazy-initialize it if it is not already
  // initialized
  if (page_size == 0) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    if (sys_info.dwPageSize > 0)
      page_size = sys_info.dwPageSize;
    else // Should never happen, but just incase use the default on x86
      page_size = 4096;
  }

  // Round the max size to a multiple of the page size.
  max_size = ((max_size + page_size - 1) / page_size) * page_size;

  a.max_size = max_size;
  a.allocated_size = page_size;
  // We reserve the page, but dont allocate it yet. Dont allow execution
  a.base = VirtualAlloc(nullptr, a.max_size, MEM_RESERVE, PAGE_READWRITE);

  SPACE_ASSERT(a.base, "Could not allocate space for arena!");

  // Allocate the first page as a optomization. Will grow the arena in time
  arena_grow(a, page_size);

  return a;
}

void arena_grow(mem_arena &arena, size_t min_size) {
  // Spin until all growth opperations are completed in other threads
  while (arena.grow_semaphore.load())
    _mm_pause();

  // Might be a bug here, should be a compare and exchange? Maybe we can force ordering with a flag?
  arena.grow_semaphore.store(true);

  // Round the size to the next page size
  size_t alloc = ((min_size + page_size - 1) / page_size) * (page_size);

  // Check if we really need to grow the arena
  if (alloc < arena.allocated_size)
    return;

  SPACE_ASSERT(alloc <= arena.max_size, "Tried to grow arena beyond its capacity!");

  // Now we can commit the already reserved memory MAKE SURE IT IS RW NOT RWX
  void *a = VirtualAlloc(arena.base, alloc, MEM_COMMIT, PAGE_READWRITE);

  arena.allocated_size = alloc;

  SPACE_ASSERT(a, "Failed to commit arena memory!")

  arena.grow_semaphore.store(false);
}

void arena_free(mem_arena &arena) {
  SPACE_ASSERT(VirtualFree(arena.base, 0, MEM_RELEASE), "Failed to free arena memory!");

  arena.base = nullptr;
  arena.size = 0;
  arena.allocated_size = 0;
}

#endif
