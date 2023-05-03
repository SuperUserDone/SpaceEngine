#include "renderer.hh"
#include "assetmanager/assetmanager.hh"
#include "common/hash.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/renderer_api.hh"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

static void draw_fullscreen_quad(app_state *state, renderer_pipeline p, renderer_texture tex) {
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  renderer_uniform u[2];
  u[0].type = UNIFORM_TYPE_MAT4;
  u[0].mat4 = mvp;
  u[0].index = 0;
  u[1].type = UNIFORM_TYPE_TEXTURE;
  u[1].texture = tex;
  u[1].index = 1;

  renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

  pipeline_settings settings;
  settings.uniforms = u;
  settings.uniform_count = 2;

  renderer_mesh *mp = &m;
  pipeline_settings *pp = &settings;

  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

static void draw_star(app_state *state, glm::vec2 pos, float raduis) {
}

static void geometry_pass(app_state *state) {
  state->api.renderer.use_framebuffer(asset_framebuffer_get_render(state, HASH_KEY("MainBuffer")));
  state->api.renderer.clear(0.0, 1.0, 1.0, 1.0);

  draw_fullscreen_quad(state,
                       asset_pipeline_get_render(state, HASH_KEY("Tonemap")),
                       asset_texture_get_render(state, HASH_KEY("MBTex")));
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
  draw_fullscreen_quad(state,
                       asset_pipeline_get_render(state, HASH_KEY("Tonemap")),
                       asset_texture_get_render(state, HASH_KEY("MBTex")));
}

void render_init(app_state *state) {
  texture_data t;
  t.w = state->window_area.w;
  t.h = state->window_area.h;
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
