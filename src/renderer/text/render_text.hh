#pragma once

#include "common/lru_cache.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "memory/memory_arena.hh"
#include "memory/memory_arena_typed.hh"

APIFUNC extern renderer_font render_font_create(app_state *state, font_data *data);
APIFUNC extern void render_font_delete(app_state *state, renderer_font font);

APIFUNC extern void render_text_init(app_state *state);
APIFUNC extern void render_text_quit(app_state *state);

APIFUNC extern void render_text(app_state *state,
                                renderer_font font,
                                uint32_t size,
                                glm::vec2 pos,
                                const char *text);

APIFUNC extern void render_font_finish(app_state *state, renderer_font font);
APIFUNC extern void render_font_reset(app_state *state, renderer_font font);

APIFUNC renderer_texture render_font_get_texture(app_state *state, renderer_font font);