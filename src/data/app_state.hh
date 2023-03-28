#pragma once

#include "common/memory_arena.hh"
#include "data/game_api.hh"
#include "game_state.hh"
#include "renderer_api.hh"

struct app_state {
  game_state game;

  void *renderer_state;
  void *platform_state;

  char working_dir[260];

  bool running;

  struct {
    renderer_api renderer;
    game_api game;
  } api;
};

typedef void (*fetch_api_fun)(app_state *state);