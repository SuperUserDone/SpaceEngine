#pragma once

#include <stdint.h>
namespace pyro {
namespace utils {

class timer {
public:
  void lt_init();
  void lt_free();

  int64_t get_us();
  int64_t reset();

private:
  int64_t m_start;
};



}
}