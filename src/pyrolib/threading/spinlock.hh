#pragma once

#include <atomic>

namespace pyro {
namespace threading {

class spinlock {
public:
  spinlock() : m_flag(true) {
  }
  void lock() noexcept {
    for (;;) {
      if (!m_flag.exchange(true, std::memory_order_acquire)) {
        return;
      }
      while (m_flag.load(std::memory_order_relaxed)) {
        _mm_pause();
      }
    }
  }

  void unlock() noexcept {
    m_flag.store(false, std::memory_order_release);
  }

private:
  std::atomic_bool m_flag;
};

} // namespace threading
} // namespace pyro