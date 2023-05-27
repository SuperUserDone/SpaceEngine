#pragma once

// This is a simple LRU cache using a ring buffer and a hashmap. I made it single header to
// hopefully allow more inlining

#include "common/hash_table.hh"
#include "memory/memory_arena.hh"

template <typename key_type, typename value_type>
struct lru_element {
  value_type val;
  key_type key;
};

template <typename key_type, typename value_type>
struct lru_cache {
  hash_table<key_type, lru_element<key_type, value_type>> index;

  lru_element<key_type, value_type> *data;

  value_type (*create_fun)(key_type key, void *userdata);
  void (*delete_fun)(value_type value, void *userdata);
  void *userdata;

  size_t data_index;
  size_t data_index_begin;
  size_t data_size;
};

template <typename key_type, typename value_type>
static inline lru_cache<key_type, value_type> lru_cache_create(
    mem_arena &arena,
    size_t size,
    value_type (*create_fun)(key_type key,
                             void *userdata),
    void (*delete_fun)(value_type value, void *userdata),
    void *userdata) {
  lru_cache<key_type, value_type> cache;

  // Create the table with 50% more elements than required for better collision behavior
  cache.index = hash_table_create<key_type, lru_element<key_type, value_type>>(arena, (size * 3) / 2);
  cache.data = (lru_element<key_type, value_type> *)arena_push(arena, sizeof(lru_element<key_type, value_type>) * size);
  cache.data_index = 0;
  cache.data_index_begin = 0;
  cache.data_size = size;
  cache.create_fun = create_fun;
  cache.delete_fun = delete_fun;
  cache.userdata = userdata;

  return cache;
}

template <typename key_type, typename value_type>
static inline void lru_evict_if_full(lru_cache<key_type, value_type> &cache) {
  if ((cache.data_index + 1) % cache.data_size == cache.data_index_begin) {
    hash_table_delete(cache.index, cache.data[cache.data_index_begin].key);
    key_delete(cache.data[cache.data_index_begin].key);
    cache.delete_fun(cache.data[cache.data_index_begin].val, cache.userdata);

    cache.data_index_begin = (cache.data_index_begin + 1) % cache.data_size;
  }
}

template <typename key_type, typename value_type>
static inline value_type *lru_cache_find(lru_cache<key_type, value_type> &cache, key_type key) {
  lru_element<key_type, value_type> *data = hash_table_search(cache.index, key);
  if (!data) {
    lru_evict_if_full(cache);
    cache.data_index = (cache.data_index + 1) % cache.data_size;

    data = &cache.data[cache.data_index];
    data->key = key_copy(key);
    data->val = cache.create_fun(data->key, cache.userdata);

    hash_table_insert(cache.index, key, data);
  }

  return &data->val;
}