#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "win32_export.hh"
#define PushStruct(arena, struct) (struct *)arena_push(arena, sizeof(struct));
#define PopStuct(arena, struct) arena_pop(arena, sizeof(struct));
#define PushArray(arena, type, len) (type *)arena_push(arena, sizeof(type) * len);

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
  if (alignment == 0) {
    // TODO error
  }

  if (a.size >= 0) {
    // TODO error
  }

  a.alignment = alignment;
}

static inline void *arena_push(mem_arena &arena, size_t size) {
  if (arena.size + size >= arena.allocated_size) {
    arena_grow(arena, arena.size + size);
  }

  size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

  void *base = (void *)((size_t)arena.base + arena.size);

  printf("Arena Alloc: %zu\n", size);

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

  printf("Arena free : %zu\n", size);
  arena.size -= size;
}

static inline void arena_pop(mem_arena &arena, void *address) {
  size_t size = (size_t)address - (size_t)arena.base;

  printf("Arena free to: %zu\n", size);
  arena.size = size;
}

static inline void arena_clear(mem_arena &arena) {
  arena.size = 0;
}