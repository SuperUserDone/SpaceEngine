#pragma once

#include "data/event.hh"

struct app_state;

typedef void (*game_init_fun)(app_state *state);
typedef void (*game_load_assets_fun)(app_state *state);
typedef void (*game_shutdown_fun)(app_state *state);

typedef void (*game_draw_debug_fun)(app_state *state);
typedef void (*game_render_fun)(app_state *state);
typedef void (*game_tick_fun)(app_state *state);
typedef void (*game_event_fun)(app_state *state, event &e);

struct game_api {
  game_init_fun init;
  game_render_fun render;
  game_tick_fun tick;
  game_shutdown_fun shutdown;
  game_event_fun event;

  game_draw_debug_fun draw_debug_info;
  game_load_assets_fun load_assets;
};
