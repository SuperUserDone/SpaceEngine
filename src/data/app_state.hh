#pragma once

#include "asset_storage.hh"
#include "data/game_api.hh"
#include "game_state.hh"
#include "memory/memory_arena.hh"
#include "renderer_api.hh"

struct render_text_state;

struct app_state {
  game_state game;
  asset_storage assets;

  mem_arena permanent_arena;
  mem_arena frame_arena;

  void *renderer_state;
  void *platform_state;
  render_text_state *render_text_state;

  char working_dir[260];

  bool running;

  int tps_target;

  struct {
    int w;
    int h;
    float dpi_scaling;
  } window_area;

  struct {
    double t;
    double dt;
  } time;

  struct {
    renderer_api renderer;
    game_api game;
  } api;
};

typedef void (*fetch_api_fun)(app_state *state);
