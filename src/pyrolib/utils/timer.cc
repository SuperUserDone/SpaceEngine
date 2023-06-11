#include "timer.hh"

#include <stdint.h>
#include <windows.h>

namespace pyro {
namespace utils {

static int64_t freq = 0;

void timer::lt_init() {
  if (freq == 0) {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);

    freq = f.QuadPart;
  }

  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  m_start = st.QuadPart;
}

void timer::lt_free() {
}

int64_t timer::get_us() {
  LARGE_INTEGER et;
  QueryPerformanceCounter(&et);
  int64_t el = et.QuadPart - m_start;

  el *= 1'000'000;

  return el / freq;
}

int64_t timer::reset() {
  LARGE_INTEGER et;
  QueryPerformanceCounter(&et);
  int64_t el = et.QuadPart - m_start;

  el *= 1'000'000;
  m_start = et.QuadPart;

  return el / freq;
}

} // namespace utils
} // namespace pyro