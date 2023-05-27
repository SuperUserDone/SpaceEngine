#pragma once

#include "common/lru_cache.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "memory/memory_arena.hh"
#include "memory/memory_arena_typed.hh"

APIFUNC extern void render_text_init(app_state *state);
APIFUNC extern void render_text_quit(app_state *state);
APIFUNC extern void render_text_newframe(app_state *state);
APIFUNC extern void render_text_queue(app_state *state, int x, int y, const char *text);
APIFUNC extern void render_text_finishframe(app_state *state);
