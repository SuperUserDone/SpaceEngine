#pragma once

#include "common/memory_arena.hh"
#include "data/game_api.hh"
#include "game_state.hh"
#include "renderer_api.hh"
#include "renderer_state.hh"

struct app_state {
  game_state game;
  renderer_state renderer;

  void *platform_state;

  bool running;

  struct {
    renderer_api renderer;
    game_api game;
  } api;
};

typedef void (*fetch_api_fun)(app_state *state);