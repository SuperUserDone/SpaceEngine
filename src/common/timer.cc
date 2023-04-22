#include "timer.hh"

#include <windows.h>

static int64_t freq;

void timer_init_subsystem() {
  LARGE_INTEGER f;
  QueryPerformanceFrequency(&f);

  freq = f.QuadPart;
}

precise_timer timer_create() {
  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);

  return {st.QuadPart};
}

int64_t timer_get_time_us(precise_timer timer) {
  LARGE_INTEGER et;
  QueryPerformanceCounter(&et);
  int64_t el = et.QuadPart - timer.start;

  el *= 1'000'000;

  return el / freq;
}

int64_t timer_reset_us(precise_timer &time) {
  LARGE_INTEGER et;
  QueryPerformanceCounter(&et);
  int64_t el = et.QuadPart - time.start;

  el *= 1'000'000;
  time.start = et.QuadPart;

  return el / freq;
}
