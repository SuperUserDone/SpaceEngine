#pragma once

// This is a simple LRU cache using a ring buffer and a hashmap. I made it single header to
// hopefully allow more inlining

#include "pyrolib/container/hash_table.hh"
#include "pyrolib/memory/arena.hh"

template <typename key_type, typename value_type>
struct lru_element {
  value_type val;
  key_type key;
};

template <typename key_type, typename value_type>
struct lru_cache {
  pyro::container::hash_table<key_type, lru_element<key_type, value_type> *> index;

  pyro::container::array<lru_element<key_type, value_type>> data;

  value_type (*create_fun)(key_type key, void *userdata);
  void (*delete_fun)(value_type value, void *userdata);
  void *userdata;

  size_t data_index;
  size_t data_index_begin;
  size_t nentries;
};

template <typename key_type, typename value_type>
static inline lru_cache<key_type, value_type> lru_cache_create(
    pyro::memory::arena &arena,
    size_t size,
    value_type (*create_fun)(key_type key, void *userdata),
    void (*delete_fun)(value_type value, void *userdata),
    void *userdata) {
  lru_cache<key_type, value_type> cache;

  // Create the table with 50% more elements than required for better collision behavior
  cache.index.lt_init((size * 3) / 2);
  cache.data.lt_init(arena, size);
  cache.data_index = 0;
  cache.data_index_begin = 0;
  cache.nentries = 0;
  cache.create_fun = create_fun;
  cache.delete_fun = delete_fun;
  cache.userdata = userdata;

  return cache;
}

template <typename key_type, typename value_type>
static inline void lru_evict_if_full(lru_cache<key_type, value_type> &cache) {
  if (cache.nentries + 1 == cache.data.size()) {
    cache.index.erase(cache.data[cache.data_index_begin].key);
    key_delete(cache.data[cache.data_index_begin].key);
    cache.delete_fun(cache.data[cache.data_index_begin].val, cache.userdata);

    cache.data_index_begin = (cache.data_index_begin + 1) % cache.data.size();
    cache.nentries--;
  }
}

template <typename key_type, typename value_type>
static inline value_type *lru_cache_find(lru_cache<key_type, value_type> &cache, key_type key) {
  auto iterator = cache.index.find(key);
  if (iterator == cache.index.end()) {
    lru_evict_if_full(cache);
    auto *data = &cache.data[cache.data_index];

    cache.data_index = (cache.data_index + 1) % cache.data.size();
    data->key = key_copy(key);
    data->val = cache.create_fun(data->key, cache.userdata);

    cache.index.insert(key, data);
    cache.nentries++;
    return &data->val;
  }

  return &iterator->val->val;
}

template <typename key_type, typename value_type>
static inline void lru_cache_invalidate(lru_cache<key_type, value_type> &cache) {
  while (cache.data_index_begin != cache.data_index) {
    lru_element<key_type, value_type> *data = &cache.data[cache.data_index_begin];
    key_delete(data->key);
    cache.data_index_begin = (cache.data_index_begin + 1) % cache.data.size();
  }

  cache.index.clear();

  cache.data_index = 0;
  cache.data_index_begin = 0;
  cache.nentries = 0;
}