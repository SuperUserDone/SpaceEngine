#pragma once

#include "common/memory_arena.hh"

struct app_state;

typedef void (*game_init_fun)(mem_arena &permanent_arena, app_state *state);
typedef void (*game_update_fun)(mem_arena &permanent_arena,
                                mem_arena &frame_arena,
                                app_state *state);
typedef void (*game_shutdown_fun)(mem_arena &permanent_arena, app_state *state);

struct game_api {
  game_init_fun init;
  game_update_fun update;
  game_shutdown_fun shutdown;
};