#pragma once

#include "common/memory_arena.hh"

struct app_state;

typedef void (*game_init_fun)(app_state *state);
typedef void (*game_load_assets_fun)(app_state *state);
typedef void (*game_update_fun)(app_state *state);
typedef void (*game_shutdown_fun)(app_state *state);

typedef void (*game_draw_debug_fun)(app_state *state);

struct game_api {
  game_init_fun init;
  game_update_fun update;
  game_shutdown_fun shutdown;

  game_draw_debug_fun draw_debug_info;
  game_load_assets_fun load_assets;
};
