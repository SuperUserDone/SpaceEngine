#pragma once

#include "common/debug.hh"
#include "pyrolib/utils/api.hh"
#include <atomic>
#include <string.h>

namespace pyro {
namespace memory {

#define DEFAULT_ARENA_MAX_SIZE 0x40000000 // 1 Gigibyte

enum class arena_push_flags : size_t {
  none = 0,
  zero = 1,
};

inline arena_push_flags operator|(arena_push_flags &a, arena_push_flags &b) {
  return static_cast<arena_push_flags>(static_cast<size_t>(a) | static_cast<size_t>(b));
}

inline bool operator&(arena_push_flags a, arena_push_flags b) {
  return static_cast<bool>(static_cast<size_t>(a) & static_cast<size_t>(b));
}

struct arena_state {
  size_t pos;
};

class PYROAPI arena {
public:
  void lt_init(size_t max_size = DEFAULT_ARENA_MAX_SIZE);
  void lt_done();
  void grow(size_t min_size);

  void pop(size_t size) {
    SPACE_ASSERT(size < m_size, "Trying to free more space in arena than available!");
    m_size.fetch_add(-size);
  }

  template <typename T>
  void pop() {
    pop(sizeof(T));
  }

  template <arena_push_flags flags = arena_push_flags::none>
  void *push(size_t size) {
    SPACE_ASSERT(m_base, "Arena base is null! Cannot allocate to null arena!");

    // Mark the memory as used
    size_t old_size = m_size.fetch_add(size);

    // Grow the arena if it is too small
    if (old_size + size >= m_allocated_size) {
      grow(old_size + size);
    }

    // Find the address of the top of the arena
    void *base = (void *)((uint8_t *)m_base + old_size);

    if (flags & arena_push_flags::zero)
      memset(base, 0, size);

    return base;
  }

  template <typename T, arena_push_flags flags = arena_push_flags::zero>
  T *push() {
    return static_cast<T *>(push<flags>(sizeof(T)));
  }

  char *push_cstring(const char *cstr, size_t len) {
    char *out = (char *)push(len + 1);
    memcpy(out, cstr, len);
    out[len] = 0;

    return out;
  }

  char *push_cstring(const char *cstr) {
    return push_cstring(cstr, strlen(cstr));
  }

  void clear() {
    m_size = 0;
  }

  arena_state get_state() {
    return {m_size};
  }

  void restore_state(arena_state ptr) {
    if (ptr.pos < m_size)
      m_size = ptr.pos;
  }

  arena() = default;

  arena(arena &&other) {
    *this = std::move(other);
  }

  arena &operator=(arena &&other) {
    this->m_base = other.m_base;
    this->m_size = other.m_size.load();
    this->m_allocated_size = other.m_allocated_size;
    this->m_max_size = other.m_max_size;
    this->m_grow_semaphore = other.m_grow_semaphore.load();

    other.m_base = nullptr;
    return *this;
  }

  arena &operator=(const arena &other) {
    *this = std::move(other);
    return *this;
  }

  void *get_base() {
    return m_base;
  }

private:
  void *m_base = nullptr;

  std::atomic_size_t m_size;
  size_t m_allocated_size;

  size_t m_max_size;

  std::atomic_bool m_grow_semaphore;
};

} // namespace memory
} // namespace pyro