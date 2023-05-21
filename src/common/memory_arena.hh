#pragma once

// This is a implementation of a memory arena. We use arenas as a supliment to the stack, so that
// we can move some things off the heap to prevent fragmentation and other issues. The other goal of
// this is to prevent heap allocations in hot paths and the main loop. In a ideal world no heap
// allocations would be neccacary, and I am trying my best to ensure that

#include <atomic>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common/debug.hh"
#include "win32_export.hh"

// Define some helpers that dont need to be their own functions.
#define arena_push_struct(arena, struct) (struct *)arena_push_zero(arena, sizeof(struct))
#define arena_pop_struct(arena, struct) arena_pop(arena, sizeof(struct))
#define arena_push_array(arena, type, len) (type *)arena_push(arena, sizeof(type) * (len))
#define arena_push_array_zero(arena, type, len) (type *)arena_push_zero(arena, sizeof(type) * (len))
#define arena_push_null_terminated_string(arena, str) arena_push_string(arena, str, strlen(str));

struct mem_arena {
  mem_arena() = default;
  mem_arena(mem_arena &&other) {
    base = other.base;
    size = other.size.load();
    allocated_size = other.allocated_size;
    max_size = other.max_size;
    alignment = other.alignment;
  };

  mem_arena &operator=(mem_arena &&other) {
    base = other.base;
    size = other.size.load();
    allocated_size = other.allocated_size;
    max_size = other.max_size;
    alignment = other.alignment;
    return *this;
  }

  void *base;
  std::atomic_size_t size;
  size_t allocated_size;
  size_t max_size;
  size_t alignment;

  std::atomic_bool grow_semaphore;

private:
  // Delete the copy ctor & copy assignment as arenas are intended to be passed by reference.
  mem_arena(const mem_arena &other) = delete;
  mem_arena &operator=(const mem_arena &other) = delete;
};

// Platform specific funtions are extracted to the cc file, as we dont want to pollute the namespace
// with platform apis and windows #defines
APIFUNC extern mem_arena arena_create(size_t max_size = 0);
APIFUNC extern void arena_grow(mem_arena &arena, size_t min_size);
APIFUNC extern void arena_free(mem_arena &arena);

static inline void arena_set_alignment(mem_arena &a, size_t alignment) {
  SPACE_ASSERT(alignment > 0, "Arena alignment must not be 0!");
  SPACE_ASSERT(a.size < 0, "Arena alignment cannot be set after creating!");

  a.alignment = alignment;
}

// The push function is declared in the header to allow more inlining and such.
static inline void *arena_push(mem_arena &arena, size_t size) {
  // Grow the arena if it is too small
  if (arena.size + size >= arena.allocated_size) {
    arena_grow(arena, arena.size + size);
  }

  // Alignment
  size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

  // Find the address of the top of the arena
  void *base = (void *)((uint8_t *)arena.base + arena.size);

  // Mark the memory as used
  arena.size += size;

  return base;
}

static inline void *arena_push_atomic(mem_arena &arena, size_t size) {
  bool done = false;
  void *base = nullptr;
  while (!done) {
    size_t arena_size = arena.size;

    // Grow the arena if it is too small
    if (arena_size + size >= arena.allocated_size) {
      arena_grow(arena, arena_size + size);
    }

    // Alignment
    size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

    // Find the address of the top of the arena
    base = (void *)((uint8_t *)arena.base + arena_size);

    // Mark the memory as used
    size_t new_arena_size = arena_size + size;

    // Store value
    done = arena.size.compare_exchange_strong(arena_size, new_arena_size);

  }


  return base;
}

static inline void *arena_push_zero(mem_arena &arena, size_t size) {
  void *data = arena_push(arena, size);

  memset(data, 0x00, size);

  return data;
}

static inline void arena_pop(mem_arena &arena, size_t size) {
  // Align the value
  size = ((size + arena.alignment - 1) / arena.alignment) * arena.alignment;

  // Mark the memory as availabe to be reused
  arena.size -= size;
}

static inline void arena_pop_to(mem_arena &arena, void *address) {
  size_t size = (size_t)address - (size_t)arena.base;

  arena.size = size;
}

static inline void arena_clear(mem_arena &arena) {
  arena.size = 0;
}

static inline char *arena_push_string(mem_arena &arena, const char *str, size_t len) {
  char *new_str = arena_push_array(arena, char, len + 1);
  memcpy(new_str, str, len);
  new_str[len] = 0;
  return new_str;
}
