#include "renderer.hh"
#include "assetmanager/assetmanager.hh"
#include "common/file_utils.hh"
#include "common/hash.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/game_state.hh"
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
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->game.camera.zoom;

  glm::mat4 cam =
      glm::ortho(-area.x, area.x, -area.y, area.y, -1.f, 1.f) *
      glm::scale(glm::translate(glm::mat4(1.f),
                                glm::vec3(pos - glm::vec2(raduis) - state->game.camera.pos, 0.f)),
                 glm::vec3(raduis * 2.0));

  renderer_uniform u[4];
  u[0].type = UNIFORM_TYPE_MAT4;
  u[0].mat4 = cam;
  u[0].index = 0;
  u[1].type = UNIFORM_TYPE_TEXTURE;
  u[1].texture = asset_texture_get_render(state, HASH_KEY("organic1"));
  u[1].index = 1;
  u[2].type = UNIFORM_TYPE_SCALAR;
  u[2].scalar = state->time.t;
  u[2].index = 2;
  u[3].type = UNIFORM_TYPE_VEC3;
  u[3].vec3 = state->game.sun_color;
  u[3].index = 3;

  renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

  pipeline_settings settings;
  settings.uniforms = u;
  settings.uniform_count = 4;

  renderer_mesh *mp = &m;
  pipeline_settings *pp = &settings;

  renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("solar"));

  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

static void geometry_pass(app_state *state) {
  state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[0]);
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);

  draw_star(state, glm::vec2(0.f), 100.f);
}

static void bloom_render_downsample(app_state *state) {
  pipeline_settings settings;
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  for (int i = 0; i < state->game.renderer.bloom_iters - 1; i++) {

    glm::vec2 window = state->game.renderer.bloom_viewports[i];
    glm::vec2 window_smaller = state->game.renderer.bloom_viewports[i + 1];
    renderer_uniform u[3];
    u[0].type = UNIFORM_TYPE_MAT4;
    u[0].mat4 = mvp;
    u[0].index = 0;
    u[1].type = UNIFORM_TYPE_TEXTURE;
    u[1].texture = state->game.renderer.bloom_buffer_textures[i];
    u[1].index = 1;
    u[2].type = UNIFORM_TYPE_VEC2;
    u[2].vec2 = window;
    u[2].index = 2;

    renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

    pipeline_settings settings;
    settings.uniforms = u;
    settings.uniform_count = 3;

    renderer_mesh *mp = &m;
    pipeline_settings *pp = &settings;

    state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[i + 1]);
    renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("bloomds"));
    state->api.renderer.set_viewport(window_smaller.x, window_smaller.y);
    state->api.renderer.clear(0.f, 0.f, 0.f, 1.f);
    state->api.renderer.draw_meshes(1, &mp, &pp, &p);
  }
}

static void bloom_render_upsample(app_state *state) {
  pipeline_settings settings;
  glm::mat4 mvp = glm::ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);

  state->api.renderer.set_blending();

  for (int i = state->game.renderer.bloom_iters - 2; i >= 0; i--) {

    glm::vec2 window = state->game.renderer.bloom_viewports[i];
    renderer_uniform u[3];
    u[0].type = UNIFORM_TYPE_MAT4;
    u[0].mat4 = mvp;
    u[0].index = 0;
    u[1].type = UNIFORM_TYPE_TEXTURE;
    u[1].texture = state->game.renderer.bloom_buffer_textures[i + 1];
    u[1].index = 1;
    u[2].type = UNIFORM_TYPE_SCALAR;
    u[2].scalar = state->game.renderer.bloom_size;
    u[2].index = 2;

    renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("Quad"));

    pipeline_settings settings;
    settings.uniforms = u;
    settings.uniform_count = 3;

    renderer_mesh *mp = &m;
    pipeline_settings *pp = &settings;

    state->api.renderer.use_framebuffer(state->game.renderer.bloom_buffers[i]);
    renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("bloomus"));
    state->api.renderer.set_viewport(window.x, window.y);
    state->api.renderer.draw_meshes(1, &mp, &pp, &p);
  }
}

static void bloom_pass(app_state *state) {
  bloom_render_downsample(state);
  bloom_render_upsample(state);
}

static void tonemap_pass(app_state *state) {
  state->api.renderer.use_default_framebuffer();
  state->api.renderer.set_viewport(state->window_area.w, state->window_area.h);
  state->api.renderer.clear(0.0, 0.0, 0.0, 1.0);
  draw_fullscreen_quad(state,
                       asset_pipeline_get_render(state, HASH_KEY("Tonemap")),
                       state->game.renderer.bloom_buffer_textures[0]);
}

static void init_bloom_textures(app_state *state) {
  glm::uvec2 window = {state->window_area.w, state->window_area.h};
  state->game.renderer.bloom_iters = MAX_BLOOM_ITERATIONS;
  for (int i = 0; i < MAX_BLOOM_ITERATIONS; i++) {
    if ((window.x < 8 || window.y < 8)) {
      float aspect = (float)window.x / (float)window.y;
      window = {8 * aspect, 8};
    }

    texture_data t;
    t.w = window.x;
    t.h = window.y;
    t.data = nullptr;
    t.format = TEX_FORMAT_RGBA_F16;
    t.upscale = TEX_FILTER_LINEAR;
    t.downscale = TEX_FILTER_LINEAR;
    t.edge = TEX_EDGE_CLAMP;
    state->game.renderer.bloom_buffer_textures[i] = state->api.renderer.create_texture(&t);

    framebuffer_data fbd;
    fbd.color_attachment = state->game.renderer.bloom_buffer_textures[i];
    state->game.renderer.bloom_buffers[i] = state->api.renderer.create_framebuffer(&fbd);
    state->game.renderer.bloom_viewports[i] = window;
    window /= 2;
  }
}

static void resize_bloom_textures(app_state *state) {
  glm::uvec2 window = {state->window_area.w, state->window_area.h};
  state->game.renderer.bloom_iters = MAX_BLOOM_ITERATIONS;

  for (int i = 0; i < MAX_BLOOM_ITERATIONS; i++) {
    if ((window.x < 8 || window.y < 8)) {
      float aspect = (float)window.x / (float)window.y;
      window = {8, 8 * aspect};
    }

    texture_data t;
    t.w = window.x;
    t.h = window.y;
    t.data = nullptr;
    t.format = TEX_FORMAT_RGBA_F16;
    t.upscale = TEX_FILTER_LINEAR;
    t.downscale = TEX_FILTER_LINEAR;
    t.edge = TEX_EDGE_CLAMP;

    state->api.renderer.update_texture(&state->game.renderer.bloom_buffer_textures[i], &t);
    state->game.renderer.bloom_viewports[i] = window;
    window /= 2;
  }
}

void render_init(app_state *state) {
  init_bloom_textures(state);

  //
  // TEMP CODE
  //

  char *sol_frag = load_file(state->frame_arena, "data/shaders/sol.frag.glsl");
  char *bloomds_frag = load_file(state->frame_arena, "data/shaders/bloomds.frag.glsl");
  char *bloomus_frag = load_file(state->frame_arena, "data/shaders/bloomus.frag.glsl");
  char *default_vert = load_file(state->frame_arena, "data/shaders/default.vert.glsl");

  pipeline_data d;
  d.vertex_shader = default_vert;
  d.fragment_shader = sol_frag;
  d.uniform_count = 4;
  const char *names[] = {"transform", "organic", "time", "sunColor"};
  d.uniform_names = names;

  asset_pipeline_create(state, HASH_KEY("solar"), &d);

  d.fragment_shader = bloomds_frag;

  d.uniform_count = 3;
  const char *bdsnames[] = {"transform", "srcTexture", "srcResolution"};
  d.uniform_names = bdsnames;

  asset_pipeline_create(state, HASH_KEY("bloomds"), &d);

  d.fragment_shader = bloomus_frag;
  d.uniform_count = 3;
  const char *busnames[] = {"transform", "srcTexture", "filterRadius"};
  d.uniform_names = busnames;

  asset_pipeline_create(state, HASH_KEY("bloomus"), &d);
}

void render_resize(app_state *state, int x, int y) {
  resize_bloom_textures(state);
}

void render_game(app_state *state) {
  geometry_pass(state);
  bloom_pass(state);
  tonemap_pass(state);
}
