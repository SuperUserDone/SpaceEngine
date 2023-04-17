#pragma once

// This is a very simple hash map implementation using an integer/hashed string as a key, and a
// pointer to a user defined piece of memory as an output. It is designed to be fast. It uses robin
// hood hashing. https://cs.uwaterloo.ca/research/tr/1986/CS-86-14.pdf (This might be a better
// resource https://dspace.mit.edu/bitstream/handle/1721.1/130693/1251799942-MIT.pdf) Allocation is
// done at the start to avoid a runtime resizing cost by forcing the user to think about how large
// the hashmap should be. (Expected + 25%)

// Memory is allocated in the given arena and freed at will with arena_clear

// I've made the hashmap header only to force some more inlining

#include "common/memory_arena.hh"
#include "win32_export.hh"
#include <type_traits>

struct hash_table_entry {
  size_t key;
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

static inline bool hash_table_insert(hash_table &t, size_t key, void *value) {
  if (t.size == t.nentries)
    return false;

  size_t location = key % t.size;
  hash_table_entry entry = {key, value, 0};

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

  return true;
};

// We use the linear search version of the hash lookup, as we do not need the better organ or smart
// lookup functions in the paper.
static inline void *hash_table_search(hash_table &t, size_t key) {
  size_t location = key % t.size;

  void *val = nullptr;
  for (size_t cost = 0; t.e[location].cost >= cost; cost++, location++) {
    location = location % t.size;

    if (t.e[location].key == key) {
      val = t.e[location].val;
      break;
    }
  }

  return val;
}
