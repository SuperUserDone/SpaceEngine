#pragma once

// This is a simple pool allocator using the arena allocator as the base. It uses a intrusive linked
// list to point to free blocks. Freed blocks are added to the front, both to aid in adding a
// minimal free and realloc cost, and to avoid wasting space. You can imagine it almost like free
// blocks are inserted into FILO queue

#include <algorithm>
#include <pyrolib/memory/arena.hh>


struct free_block {
  free_block *next;
};

template <typename T>
struct mem_pool {
  pyro::memory::arena arena;
  size_t obj_size;
  free_block *first_free;
};

template <typename T>
static inline mem_pool<T> pool_create(size_t max_elements) {
  mem_pool<T> m;
  m.arena.lt_init(max_elements * sizeof(T));
  // Some datastructures and primitives can be smaller a void*. Account fot that
  m.obj_size = std::max(sizeof(T), sizeof(free_block));
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
    return static_cast<T *>(pool.arena.push(pool.obj_size));
  }
}

template <typename T>
static inline void pool_pop(mem_pool<T> &pool, T *addr) {
  // When freeing add it to the list for the next allocation.
  void *old_free = pool.first_free;
  ((free_block *)addr)->next = (free_block *)old_free;
  pool.first_free = (free_block *)addr;
}

template <typename T>
static inline void pool_free(mem_pool<T> &pool) {
  // Call the underlying functions.
  pool.arena.lt_done();
}

