#pragma once

#include "data/app_state.hh"

extern void render_game(app_state *state);
extern void render_init(app_state *state);
extern void render_resize(app_state *state, int x, int y);
extern void render_cleanup(app_state *state);
