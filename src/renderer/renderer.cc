#include "renderer.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/renderer_api.hh"

static void draw_star(app_state *state, glm::vec2 pos, float raduis) {
}

static void geometry_pass(app_state *state) {
  state->api.renderer.use_framebuffer(asset_framebuffer_get_render(state, HASH_KEY("MainBuffer")));
  state->api.renderer.clear(0.0, 1.0, 1.0, 1.0);
}

static void bloom_pass(app_state *state) {
  /*glm::uvec2 window = {state->window_area.w, state->window_area.h};

  while (window.x > 10 && window.y > 10)
  {
    window /= 2;
  } */
}

static void tonemap_pass(app_state *state) {
  state->api.renderer.use_default_framebuffer();
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);
}

void render_init(app_state *state) {
  texture_data t;
  t.w = 800;
  t.h = 600;
  t.data = nullptr;
  t.format = TEX_FORMAT_RGBA_F16;
  asset_texture_create(state, HASH_KEY("MBTex"), &t);

  framebuffer_data d;
  d.color_attachment = asset_texture_get_render(state, HASH_KEY("MBTex"));
  asset_framebuffer_create(state, HASH_KEY("MainBuffer"), &d);
}

void render_resize(app_state *state, int x, int y) {
  texture_data t;
  t.w = x;
  t.h = y;
  t.data = nullptr;
  t.format = TEX_FORMAT_RGBA_F16;
  asset_texture_update(state, HASH_KEY("MBTex"), &t);
}

void render_game(app_state *state) {
  geometry_pass(state);
  bloom_pass(state);
  tonemap_pass(state);
}
