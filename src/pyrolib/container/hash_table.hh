#pragma once

// This is a very simple hash map implementation using an integer/hashed string as a key, and a
// pointer to a user defined piece of memory as an output. It is designed to be fast. It uses robin
// hood hashing from this paper https://cs.uwaterloo.ca/research/tr/1986/CS-86-14.pdf (This might be
// a better resource to understand the basic working of the implementation
// https://dspace.mit.edu/bitstream/handle/1721.1/130693/1251799942-MIT.pdf)

#include "common/hash.hh"
#include "pyrolib/container/array.hh"
#include "pyrolib/memory/arena.hh"
#include "pyrolib/memory/heap.hh"
#include "pyrolib/utils/compare.hh"
#include "pyrolib/utils/hash.hh"
#include <iterator>
#include <stdint.h>
#include <unordered_map>
#include <vcruntime_string.h>

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

namespace pyro {
namespace container {

template <typename key_type,
          typename val_type,
          typename hasher = utils::hasher<key_type>,
          typename comparison = utils::comparison<key_type>>
struct hash_table {
public:
  struct iterator;
  void lt_init(size_t size = 0x10) {
    m_growable = true;
    m_used_entries = 0;

    memory::heap heap;
    m_entries.lt_init(heap, size);
    memset(&m_entries[0], 0x00, m_entries.size());
  }

  void lt_init_static(memory::arena &arena, size_t size) {
    m_growable = false;
    m_used_entries = 0;

    m_entries.lt_init(arena, size);
    memset(&m_entries[0], 0x00, m_entries.size() * sizeof(hash_table_entry));
  }
  void lt_done() {
    if (m_growable) {
      memory::heap heap;
      m_entries.lt_done(heap);
    }
  }

  iterator insert(const key_type &key, const val_type &val) {
    if (m_entries.size() == m_used_entries)
      return end();

    size_t hash = hasher::hash(key);

    if (hash == 0) {
      printf("Err");
    }

    size_t location = hash % m_entries.size();

    hash_table_entry entry = {key_copy(key), hash, val, 0, true};

    while (m_entries[location].set) {
      if (m_entries[location].hash == hash && comparison::compare(key, m_entries[location].key)) {
        key_delete(m_entries[location].key);
        m_entries[location] = entry;

        return iterator(&m_entries[location], m_entries.end());
      }

      if (entry.cost > m_entries[location].cost) {
        hash_table_entry temp = m_entries[location];
        m_entries[location] = entry;
        entry = temp;
      }
      entry.cost++;
      location = (location + 1) % m_entries.size();
    }
    m_entries[location] = entry;
    m_used_entries++;
  
    return iterator(&m_entries[location], m_entries.end());
  }

  // We use the linear search version of the hash lookup, as we do not need the better organ or
  // smart lookup functions in the paper.
  iterator find(key_type key) {
    size_t hash = hasher::hash(key);
    size_t location = hash % m_entries.size();

    iterator loc = end();
    for (size_t cost = 0; m_entries[location].cost >= cost; cost++, location++) {
      location = location % m_entries.size();

      if (m_entries[location].hash == hash && comparison::compare(m_entries[location].key, key)) {
        loc = iterator(&m_entries[location], m_entries.end());
        break;
      }
    }

    return loc;
  }

  void erase(key_type key) {
    iterator pos = find(key);
    if (pos == end())
      return;

    size_t loc = static_cast<size_t>(pos.get() - &m_entries[0]);

    key_delete(m_entries[loc].key);
    while (true) {
      size_t pl = loc;
      loc = (loc + 1) % m_entries.size();
      if (m_entries[loc].cost > 0) {
        m_entries[pl] = m_entries[loc];
        m_entries[pl].cost--;
      } else {
        m_entries[pl] = hash_table_entry{0, 0, {}, 0, false};
        m_used_entries--;
        break;
      }
    }
  }

  void clear() {
    for (size_t i = 0; i < m_entries.size(); i++) {
      if (m_entries[i].set) {
        key_delete(m_entries[i].key);
        m_entries[i] = hash_table_entry{0, 0, {}, 0, false};
      }
    }

    m_used_entries = 0;
  }

  size_t size() {
    return m_used_entries;
  }

  struct hash_table_entry {
    key_type key;
    size_t hash;
    val_type val;
    int32_t cost;
    bool set;
  };

private:
  bool m_growable;
  size_t m_used_entries;

  pyro::container::array<hash_table_entry> m_entries;

public:
  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = hash_table_entry;
    using pointer = value_type *;   // or also value_type*
    using reference = value_type &; // or also value_type&

    iterator(pointer data, pointer end)
        : m_ptr(data), m_end(end){

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
      } while (!m_ptr->set && m_ptr != m_end);

      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    pointer get() {
      return m_ptr;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.m_ptr != b.m_ptr;
    };

  private:
    pointer m_ptr;
    pointer m_end;
  };

  iterator begin() {
    auto entry = m_entries.begin();
    while (!entry->set && entry != m_entries.end()) {
      entry++;
    }
    return iterator(entry, m_entries.end());
  }

  iterator end() {
    return iterator(m_entries.end(), m_entries.end());
  }
};

} // namespace container
} // namespace pyro