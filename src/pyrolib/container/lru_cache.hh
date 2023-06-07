#pragma once

#include "pyrolib/container/hash_table.hh"
#include "pyrolib/memory/arena.hh"

namespace pyro {
namespace container {

template <typename key_type, typename value_type, typename builder_type>
class lru_cache {
public:
  void lt_init(memory::arena &arena, size_t size, builder_type *builder = nullptr) {
    m_index.lt_init_static(arena, size * 1.5);
    m_data.lt_init(arena, size);

    m_data_index_begin = 0;
    m_data_index = 0;
    m_nentries = 0;
    m_builder = builder;
  }

  void lt_done() {
    m_index.lt_done();
    m_data.lt_done();
  }

  value_type *find_or_insert(key_type key) {
    auto iterator = m_index.find(key);
    if (iterator == m_index.end()) {
      evict_if_full();
      auto *data = &m_data[m_data_index];

      m_data_index = (m_data_index + 1) % m_data.size();
      data->key = key_copy(key);
      data->val = m_builder->create(data->key);

      m_index.insert(key, data);
      m_nentries++;
      return &data->val;
    }

    return &iterator->val->val;
  }

  void clear() {
    while (m_data_index_begin != m_data_index) {
      lru_element *data = &m_data[m_data_index_begin];
      key_delete(data->key);
      m_data_index_begin = (m_data_index_begin + 1) % m_data.size();
    }

    m_index.clear();

    m_data_index = 0;
    m_data_index_begin = 0;
    m_nentries = 0;
  }

  size_t size() {
    return m_nentries;
  }

  size_t ht_size() {
    return m_index.size();
  }

private:
  struct lru_element;
  pyro::container::hash_table<key_type, lru_element *> m_index;
  pyro::container::array<lru_element> m_data;

  size_t m_data_index;
  size_t m_data_index_begin;
  size_t m_nentries;

  builder_type *m_builder;

  void evict_if_full() {
    if (m_nentries + 1 == m_data.size()) {
      m_index.erase(m_data[m_data_index_begin].key);
      key_delete(m_data[m_data_index_begin].key);
      m_builder->cleanup(m_data[m_data_index_begin].val);

      m_data_index_begin = (m_data_index_begin + 1) % m_data.size();
      m_nentries--;
    }
  }

  struct lru_element {
    value_type val;
    key_type key;
  };
};
} // namespace container
} // namespace pyro