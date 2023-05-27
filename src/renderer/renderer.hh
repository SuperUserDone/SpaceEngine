#pragma once

#include "data/app_state.hh"

APIFUNC extern void render_game(app_state *state);
APIFUNC extern void render_init(app_state *state);
APIFUNC extern void render_resize(app_state *state, int x, int y);
APIFUNC extern void render_cleanup(app_state *state);
