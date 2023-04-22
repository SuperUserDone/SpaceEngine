#pragma once 

#include "common/win32_export.hh"
#include <stdint.h>

struct precise_timer {
  int64_t start;
};
APIFUNC extern void timer_init_subsystem();

APIFUNC extern precise_timer timer_create();

APIFUNC extern int64_t timer_get_time_us(precise_timer time);
APIFUNC extern int64_t timer_reset_us(precise_timer &time);
