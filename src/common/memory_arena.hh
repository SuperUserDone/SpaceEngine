#pragma once

// This is a implementation of a memory arena based upon the pattern as laid out by ryan fleury on
// his blog. We use arenas as a supliment to the stack, so that we can move some things off the heap
// to prevent fragmentation and other issues. The other goal of this is to prevent heap allocations
// in hot paths and the main loop.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common/debug.hh"
#include "win32_export.hh"

#define arena_push_struct(arena, struct) (struct *)arena_push_zero(arena, sizeof(struct))
#define arena_pop_struct(arena, struct) arena_pop(arena, sizeof(struct))
#define arena_push_array(arena, type, len) (type *)arena_push(arena, sizeof(type) * (len))
#define arena_push_array_zero(arena, type, len) (type *)arena_push_zero(arena, sizeof(type) * (len))

struct mem_arena {
  void *base;
  size_t size;
  size_t allocated_size;
  size_t max_size;
  size_t alignment;
};

APIFUNC extern mem_arena arena_create(size_t max_size = 0);
APIFUNC extern void arena_grow(mem_arena &arena, size_t min_size);
APIFUNC extern void arena_free(mem_arena &arena);

static inline void arena_set_alignment(mem_arena &a, size_t alignment) {
  SPACE_ASSERT(alignment > 0, "Arena alignment must not be 0!");

  SPACE_ASSERT(a.size < 0, "Arena alignment cannot be set after creating!");

  a.alignment = alignment;
}

static inline void *arena_push(mem_arena &arena, size_t size) {
  if (arena.size + size >= arena.allocated_size) {
    arena_grow(arena, arena.size + size);
  }

  size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

  void *base = (void *)((size_t)arena.base + arena.size);

  arena.size += size;

  return base;
}

static inline void *arena_push_zero(mem_arena &arena, size_t size) {
  void *data = arena_push(arena, size);

  memset(data, 0x00, size);

  return data;
}

static inline void arena_pop(mem_arena &arena, size_t size) {
  size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

  arena.size -= size;
}

static inline void arena_pop_to(mem_arena &arena, void *address) {
  size_t size = (size_t)address - (size_t)arena.base;

  arena.size = size;
}

static inline void arena_clear(mem_arena &arena) {
  arena.size = 0;
}
