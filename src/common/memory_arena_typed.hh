#pragma once

#include "common/memory_arena.hh"

template <typename T>
struct mem_arena_typed {
  mem_arena arena;
};

template <typename T>
static inline mem_arena_typed<T> arena_typed_create(size_t max_elements) {
  mem_arena_typed<T> mem;

  mem.arena = arena_create(max_elements * sizeof(T));
}

template <typename T>
static inline void arena_typed_free(mem_arena_typed<T> &arena) {
  arena_free(arena);
}

template <typename T>
static inline T *arena_typed_push(mem_arena_typed<T> &array) {
  return arena_push_struct(array.arena, T);
}

template <typename T>
static inline void arena_typed_pop(mem_arena_typed<T> &array) {
  arena_pop_struct(array.arena, T);
}

template <typename T>
static inline void arena_typed_pop_to(mem_arena_typed<T> &array, void *addr) {
  arena_pop_to(array.arena, addr);
}

template <typename T>
static inline void arena_typed_clear(mem_arena_typed<T> &array) {
  arena_clear(array.arena);
}