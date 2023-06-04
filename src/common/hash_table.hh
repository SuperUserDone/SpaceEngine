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
#include "common/hash_table.hh"
#include "memory/memory_arena.hh"
#include "win32_export.hh"
#include <string.h>

template <typename key_type, typename val_type>
struct hash_table_entry {
  key_type key;
  size_t hash;
  val_type *val;
  int32_t cost;
};

template <typename key_type, typename val_type>
struct hash_table {
  hash_table_entry<key_type, val_type> *e;
  size_t nentries;

  size_t (*hash_fun)(key_type type);
  bool (*comp_fun)(key_type l, key_type r);
  size_t size;
};

template <typename T>
static inline size_t hash_table_default_hash(T in) {
  return (size_t)XXH3_64bits(&in, sizeof(in));
}

template <>
inline size_t hash_table_default_hash<const char *>(const char *string) {
  return HASH_KEY(string);
}

template <typename T>
static inline bool hash_table_default_compare(T a, T b) {
  return a == b;
}

template <>
inline bool hash_table_default_compare<const char *>(const char *string1, const char *string2) {
  return strcmp(string1, string2) == 0;
}

template <typename key_type = const char *, typename val_type = void *>
static inline hash_table<key_type, val_type> hash_table_create(
    mem_arena &a,
    size_t size = 0xffff,
    size_t (*hash_fun)(key_type type) = nullptr,
    bool (*comp_fun)(key_type l, key_type r) = nullptr) {

  if (!hash_fun)
    hash_fun = hash_table_default_hash<key_type>;
  if (!comp_fun)
    comp_fun = hash_table_default_compare<key_type>;

  return {(hash_table_entry<key_type, val_type> *)
              arena_push_zero(a, sizeof(hash_table_entry<key_type, val_type>) * size),
          0,
          hash_fun,
          comp_fun,
          size};
}

template <typename key_type>
static inline key_type key_copy(key_type in) {
  return in;
}

template <>
inline const char *key_copy<const char *>(const char *in) {
  size_t key_len = strlen(in);
  char *key_copy = new char[key_len + 1];
  strcpy_s(key_copy, key_len + 1, in);

  return key_copy;
}

template <typename key_type>
static inline void key_delete(key_type in) {
}

template <>
inline void key_delete<const char *>(const char *in) {
  delete[] in;
}

template <typename key_type, typename val_type>
static inline bool hash_table_insert(hash_table<key_type, val_type> &t,
                                     key_type key,
                                     val_type *value) {
  if (t.size == t.nentries)
    return false;

  size_t hash = t.hash_fun(key);

  if (hash == 0) {
    printf("Err");
  }

  size_t location = hash % t.size;

  hash_table_entry<key_type, val_type> entry = {key_copy(key), hash, value, 0};

  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc >= 0) {
    key_delete(t.e[loc].key);
    t.e[loc] = entry;
    return true;
  }

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

template <typename key_type, typename val_type>
static inline int64_t _hash_table_get_key_location(hash_table<key_type, val_type> &t,
                                                   key_type key) {
  size_t hash = t.hash_fun(key);

  size_t location = hash % t.size;

  int64_t loc = -1;
  for (size_t cost = 0; t.e[location].cost >= cost; cost++, location++) {
    location = location % t.size;

    if (t.e[location].hash == hash && t.comp_fun(t.e[location].key, key)) {
      loc = location;
      break;
    }
  }

  return loc;
}

// We use the linear search version of the hash lookup, as we do not need the better organ or smart
// lookup functions in the paper.
template <typename key_type, typename val_type>
static inline val_type *hash_table_search(hash_table<key_type, val_type> &t, key_type key) {
  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc >= 0) {
    return t.e[loc].val;
  } else {
    return nullptr;
  }
}

template <typename key_type, typename val_type>
static inline void hash_table_delete(hash_table<key_type, val_type> &t, key_type key) {
  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc < 0)
    return;

  key_delete(t.e[loc].key);
  while (true) {
    size_t pl = loc;
    loc = (loc + 1) % t.size;
    if (t.e[loc].cost > 0) {
      t.e[pl] = t.e[loc];
      t.e[pl].cost--;
    } else {
      t.e[pl] = hash_table_entry<key_type, val_type>{0, 0, 0, 0};
      t.nentries--;
      break;
    }
  }
}

template <typename key_type, typename val_type>
static inline void hash_table_clear(hash_table<key_type, val_type> &t) {
  for (size_t i = 0; i < t.size; i++) {
    if (t.e[i].val != nullptr) {
      key_delete(t.e[i].key);
      t.e[i] = hash_table_entry<key_type, val_type>{0, 0, 0, 0};
    }
  }

  t.nentries = 0;
}
