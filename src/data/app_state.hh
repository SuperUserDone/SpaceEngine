#pragma once

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
  } api;
};