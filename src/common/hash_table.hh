#pragma once

// This is a very simple hash map implementation using an integer/hashed string as a key, and a
// pointer to a user defined piece of memory as an output. It is designed to be fast. It uses robin
// hood hashing from this paper https://cs.uwaterloo.ca/research/tr/1986/CS-86-14.pdf (This might be
// a better resource to understand the basic working of the implementation
// https://dspace.mit.edu/bitstream/handle/1721.1/130693/1251799942-MIT.pdf) Allocation is done at
// the start to avoid a runtime resizing cost by forcing the user to think about how large the
// hashmap should be. (Expected + 15% Max in my testing, depends on colision frequency)

// Memory is allocated in the given arena and freed at will with arena_clear

// I've made the hashmap header only to force some more inlining

#include "common/hash.hh"
#include "memory/memory_arena.hh"
#include "win32_export.hh"
#include <string.h>

struct hash_table_entry {
  const char *key;
  size_t hash;
  void *val;
  int32_t cost;
};

struct hash_table {
  hash_table_entry *e;
  size_t nentries;
  size_t size;
};

static inline hash_table hash_table_create(mem_arena &a, size_t size = 0xffff) {
  return {arena_push_array_zero(a, hash_table_entry, size), 0, size};
}

static inline bool hash_table_insert(hash_table &t, const char *key, void *value) {
  if (t.size == t.nentries)
    return false;

  size_t hash = HASH_KEY(key);

  size_t location = hash % t.size;
  hash_table_entry entry = {key, hash, value, 0};

  while (t.e[location].val != nullptr) {
    if (entry.cost > t.e[location].cost) {
      hash_table_entry temp = t.e[location];
      t.e[location] = entry;
      entry = temp;
    }
    entry.cost++;
    location = (location + 1) % t.size;
  }
  t.e[location] = entry;
  t.nentries++;
  return true;
};

static inline int64_t _hash_table_get_key_location(hash_table &t, const char *key) {
  size_t hash = HASH_KEY(key);

  size_t location = hash % t.size;

  int64_t loc = -1;
  for (size_t cost = 0; t.e[location].cost >= cost; cost++, location++) {
    location = location % t.size;

    if (t.e[location].hash == hash && key[0] == t.e[location].key[0] &&
        (strcmp(key + 1, t.e[location].key + 1) == 0)) {
      loc = location;
      break;
    }
  }

  return loc;
}

// We use the linear search version of the hash lookup, as we do not need the better organ or smart
// lookup functions in the paper.
static inline void *hash_table_search(hash_table &t, const char *key) {
  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc >= 0) {
    return t.e[loc].val;
  } else {
    return nullptr;
  }
}

static inline void hash_table_delete(hash_table &t, const char *key) {
  int64_t loc = _hash_table_get_key_location(t, key);
  while (loc >= 0) {
    size_t pl = loc;
    loc = (loc + 1) % t.size;
    if (t.e[loc].cost >= t.e[pl].cost && t.e[loc].cost > 0) {
      t.e[pl] = t.e[loc];
      t.e[pl].cost--;
    } else {
      t.e[pl] = hash_table_entry{0, 0, 0, 0};
      break;
    }
  }
}
