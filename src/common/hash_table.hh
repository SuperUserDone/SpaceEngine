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
#include "win32_export.hh"
#include <iterator>
#include <pyrolib/container/array.hh>
#include <pyrolib/memory/arena.hh>
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
  pyro::container::array<hash_table_entry<key_type, val_type>> entries;
  size_t nentries;

  size_t (*hash_fun)(key_type type);
  bool (*comp_fun)(key_type l, key_type r);

  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = hash_table_entry<key_type, val_type>;
    using pointer = value_type *;   // or also value_type*
    using reference = value_type &; // or also value_type&

    iterator(pointer data)
        : m_ptr(data){

          };

    reference operator*() const {
      return *m_ptr;
    }
    pointer operator->() {
      return m_ptr;
    }

    iterator &operator++() {
      do {
        m_ptr++;
      } while (m_ptr->val == nullptr);

      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.m_ptr != b.m_ptr;
    };

  private:
    pointer m_ptr;
  };

  iterator begin() {
    auto entry = entries.begin();
    while (entry->val == nullptr && entry != end()) {
      entry++;
    }
    return iterator(entry);
  }

  iterator end() {
    return iterator(entries.end());
  }
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
    pyro::memory::arena &a,
    size_t size = 0xffff,
    size_t (*hash_fun)(key_type type) = nullptr,
    bool (*comp_fun)(key_type l, key_type r) = nullptr) {

  if (!hash_fun)
    hash_fun = hash_table_default_hash<key_type>;
  if (!comp_fun)
    comp_fun = hash_table_default_compare<key_type>;

  hash_table<key_type, val_type> t = {{}, 0, hash_fun, comp_fun};

  t.entries.lt_init(a, size);

  return t;
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
  if (t.entries.size() == t.nentries)
    return false;

  size_t hash = t.hash_fun(key);

  if (hash == 0) {
    printf("Err");
  }

  size_t location = hash % t.entries.size();

  hash_table_entry<key_type, val_type> entry = {key_copy(key), hash, value, 0};

  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc >= 0) {
    key_delete(t.entries[loc].key);
    t.entries[loc] = entry;
    return true;
  }

  while (t.entries[location].val != nullptr) {
    if (entry.cost > t.entries[location].cost) {
      hash_table_entry temp = t.entries[location];
      t.entries[location] = entry;
      entry = temp;
    }
    entry.cost++;
    location = (location + 1) % t.entries.size();
  }
  t.entries[location] = entry;
  t.nentries++;
  return true;
};

template <typename key_type, typename val_type>
static inline int64_t _hash_table_get_key_location(hash_table<key_type, val_type> &t,
                                                   key_type key) {
  size_t hash = t.hash_fun(key);

  size_t location = hash % t.entries.size();

  int64_t loc = -1;
  for (size_t cost = 0; t.entries[location].cost >= cost; cost++, location++) {
    location = location % t.entries.size();

    if (t.entries[location].hash == hash && t.comp_fun(t.entries[location].key, key)) {
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
    return t.entries[loc].val;
  } else {
    return nullptr;
  }
}

template <typename key_type, typename val_type>
static inline void hash_table_delete(hash_table<key_type, val_type> &t, key_type key) {
  int64_t loc = _hash_table_get_key_location(t, key);
  if (loc < 0)
    return;

  key_delete(t.entries[loc].key);
  while (true) {
    size_t pl = loc;
    loc = (loc + 1) % t.entries.size();
    if (t.entries[loc].cost > 0) {
      t.entries[pl] = t.entries[loc];
      t.entries[pl].cost--;
    } else {
      t.entries[pl] = hash_table_entry<key_type, val_type>{0, 0, 0, 0};
      t.nentries--;
      break;
    }
  }
}

template <typename key_type, typename val_type>
static inline void hash_table_clear(hash_table<key_type, val_type> &t) {
  for (size_t i = 0; i < t.entries.size(); i++) {
    if (t.entries[i].val != nullptr) {
      key_delete(t.entries[i].key);
      t.entries[i] = hash_table_entry<key_type, val_type>{0, 0, 0, 0};
    }
  }

  t.nentries = 0;
}
