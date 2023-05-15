#pragma once

// This is a simple pool allocator using the arena allocator as the base. It uses a intrusive linked
// list to point to free blocks. Freed blocks are added to the front, both to aid in adding a
// minimal free and realloc cost, and to avoid wasting space. You can imagine it almost like free
// blocks are inserted into FILO queue

#include "memory_arena.hh"
#include "win32_export.hh"
#include <math.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))

struct free_block {
  free_block *next;
};

template <typename T>
struct mem_pool {
  mem_pool() = default;

  mem_arena arena;
  size_t obj_size;
  free_block *first_free;

private:
  // Delete the copy ctor as arenas are intended to be passed by reference.
  mem_pool(const mem_arena &other) = delete;
};

template <typename T>
static inline mem_pool<T> pool_create(size_t max_elements) {
  mem_pool<T> m;
  m.arena = arena_create(max_elements * sizeof(T));
  // Some datastructures and primitives can be smaller a void*. Account fot that
  m.obj_size = max(sizeof(T), sizeof(free_block));
  m.first_free = nullptr;
  return m;
}

template <typename T>
static inline T *pool_alloc(mem_pool<T> &pool) {
  // Try to allocate using freed blocks rather than new memory
  if (pool.first_free) {
    free_block *addr = pool.first_free;
    pool.first_free = pool.first_free->next;
    return (T *)addr;
  } else {
    // If no free blocks exist then allocate it on the arena
    return (T *)arena_push(pool.arena, pool.obj_size);
  }
}

template <typename T>
static inline void pool_pop(mem_pool<T> &pool, T *addr) {
  // When freeing add it to the list for the next allocation.
  void *old_free = pool.first_free;
  ((free_block *)addr)->next = (free_block *)old_free;
  pool.first_free = (free_block *)addr;
}

// Helper function. Indicies should be stable, but can be reassigned to a different allocation if
// it was freed. Avoid if you are unsure if the object is freed.
template <typename T>
static inline T *pool_get_at_index(mem_pool<T> &pool, size_t index) {
  return ((T *)pool.arena.base) + index;
}

template <typename T>
static inline void pool_free(mem_pool<T> &pool) {
  // Call the underlying functions.
  arena_free(pool.arena);
}

// Helper function. Indicies should be stable, but can be reassigned to a different allocation if
// it was freed. Avoid if you are unsure if the object is freed.
template <typename T>
static inline size_t pool_get_index(mem_pool<T> &pool, T *addr) {
  size_t offset = (size_t)addr - (size_t)pool.arena.base;

  return offset / pool.obj_size;
}
