#pragma once

#include "api.hh"
#include <stdint.h>

namespace pyro {
namespace utils {

class PYROAPI timer {
public:
  void lt_init();
  void lt_done();

  int64_t get_us();
  int64_t reset();

private:
  int64_t m_start;
};

} // namespace utils
} // namespace pyro