#include "memory_arena.hh"

#ifdef _WIN32

#include <windows.h>

static size_t page_size = 0x0;
static const size_t max_arena_size_default = 0x40000000; // 1 Gigibyte

mem_arena arena_create(size_t max_size) {
  mem_arena a;

  if (max_size == 0)
    max_size = max_arena_size_default;

  a.alignment = 1;
  a.base = nullptr;
  a.size = 0;

  if (page_size == 0) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    if (sys_info.dwPageSize > 0)
      page_size = sys_info.dwPageSize;
    else
      page_size = 4096;
  }

  max_size = ((max_size + page_size - 1) / page_size) * page_size;

  a.max_size = max_size;
  a.allocated_size = page_size;
  a.base = VirtualAlloc(nullptr, a.max_size, MEM_RESERVE, PAGE_READWRITE);

  if (!a.base) {
    // TODO Error
  }

  arena_grow(a, max_size);

  return a;
}

void arena_grow(mem_arena &arena, size_t min_size) {
  size_t alloc = ((min_size + page_size - 1) / page_size) * (page_size);
  if (alloc >= arena.max_size) {
    // TODO Error
  }

  void *a = VirtualAlloc(arena.base, alloc, MEM_COMMIT, PAGE_READWRITE);
  if (!a) {
    // TODO Error
  }
}

void arena_free(mem_arena &arena) {
  if (!VirtualFree(arena.base, arena.max_size, MEM_FREE)) {
    // TODO Error
  }

  arena.base = nullptr;
  arena.size = 0;
  arena.allocated_size = 0;
}

#endif
