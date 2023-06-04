#pragma once

#include "common/lru_cache.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "memory/memory_arena.hh"
#include "memory/memory_arena_typed.hh"

#define FONT_SIZE_MIN 1 
#define FONT_SIZE_MAX 100 
#define FONT_MAX_GLYPH_COUNT 65536 
#define FONT_MIN_GLYPH_ATLAS_SIZE 1024 
#define FONT_MAX_GLYPH_ATLAS_SIZE 8192 
#define FONT_ATLAS_RESIZE_THRES 3

struct render_font_info {
    glm::uvec2 atlas_size;
    size_t ht_entries;
    size_t cache_entries;
    renderer_texture texture;
};

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

APIFUNC render_font_info render_font_get_info(app_state *state, renderer_font font);