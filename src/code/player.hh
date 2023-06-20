#pragma once 

#include "data/app_state.hh"

static inline void render_player(app_state *state) {
  rect r;
  r.pos = state->game.gameplay.player_pos;
  r.size = {32, 32};
  r.uva = {0, 0};
  r.uvb = {1, 1};
  render_batch_add_rect(state->game.b, r);
}

