#pragma once

#include "memory_arena.hh"
#include "win32_export.hh"
#include <math.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))

struct free_block {
  free_block *next;
};

template <typename T>
struct mem_pool {
  mem_arena arena;
  size_t obj_size;
  free_block *first_free;
};

template <typename T>
static inline mem_pool<T> pool_create(size_t max_size) {
  mem_pool<T> m;
  m.arena = arena_create(max_size);
  m.obj_size = max(sizeof(T), sizeof(free_block));
  m.first_free = nullptr;
  return m;
}

template <typename T>
static inline T *pool_alloc(mem_pool<T> &pool) {
  if (pool.first_free) {
    free_block *addr = pool.first_free;
    pool.first_free = pool.first_free->next;
    return (T *)addr;
  } else {
    return (T *)arena_push(pool.arena, pool.obj_size);
  }
}

template <typename T>
static inline void pool_free(mem_pool<T> &pool, T *addr) {
  void *old_free = pool.first_free;
  ((free_block *)addr)->next = (free_block *)old_free;
  pool.first_free = (free_block *)addr;
}