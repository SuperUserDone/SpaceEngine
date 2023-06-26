#pragma once

#include "pyrolib/memory/arena.hh"
#include "pyrolib/memory/heap.hh"

namespace pyro {
namespace container {

template <typename T>
class array {
public:
  typedef T type;
  typedef array<T> this_type;

  void lt_init(T *base, size_t size) {
    m_base = base;
    m_size = size;
  }

  void lt_init(memory::arena &arena, size_t size) {
    m_base = static_cast<T *>(arena.push<memory::arena_push_flags::zero>(sizeof(T) * size));
    m_size = size;
  }

  void lt_init(memory::heap &heap, size_t size) {
    m_base = static_cast<T *>(heap.alloc(sizeof(T) * size));
    m_size = size;
  }

  void lt_done(memory::heap &heap)
  {
    heap.free(m_base);
    m_base = nullptr;
  }

  void lt_copy(memory::arena &arena, const array<T> &arr) {
    lt_init(arena, arr.size());
    memcpy(m_base, &arr[0], arr.size() * sizeof(T));
  }

  template <typename U>
  void lt_copy_from(memory::arena &arena, const U &other) {
    static_assert(std::is_same<typename U::type, type>::value, "Incompatable array types!");
    lt_init(arena, other.size());
    memcpy(m_base, &other[0], other.size() * sizeof(T));
  }

  typedef T *iterator;

  T &operator*() {
    return *this->m_base;
  }

  T *operator->() {
    return this->m_base;
  }

  T &operator[](size_t index) {
    return m_base[index];
  }

  T &operator[](size_t index) const {
    return m_base[index];
  }

  size_t size() const {
    return m_size;
  }

  iterator begin() {
    return m_base;
  }

  iterator end() {
    return &m_base[m_size];
  }

private:
  T *m_base;
  size_t m_size;
};
} // namespace container
} // namespace pyro