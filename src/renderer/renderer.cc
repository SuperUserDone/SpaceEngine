#include "renderer.hh"
#include "assetmanager/assetmanager.hh"
#include "common/file_utils.hh"
#include "common/hash.hh"
#include "common/memory_arena.hh"
#include "common/pipeline_settings.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/game_state.hh"
#include "data/renderer_api.hh"
#include "renderer/render_pass_bloom.hh"
#include "renderer/render_pass_geometry.hh"
#include "renderer/render_pass_tonemap.hh"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

void render_init(app_state *state) {
  render_pass_bloom_init(state);
  state->api.renderer.set_blending(BLEND_ONE_MIN_SRC_ALPHA);

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

  d.uniform_count = 5;
  const char *bdsnames[] = {"transform", "srcTexture", "srcResolution", "bloomParams", "firstpass"};
  d.uniform_names = bdsnames;

  asset_pipeline_create(state, HASH_KEY("bloomds"), &d);

  d.fragment_shader = bloomus_frag;
  d.uniform_count = 3;
  const char *busnames[] = {"transform", "srcTexture", "filterRadius"};
  d.uniform_names = busnames;

  asset_pipeline_create(state, HASH_KEY("bloomus"), &d);
}

void render_resize(app_state *state, int x, int y) {
  render_pass_bloom_resize(state);
}

void render_calculate_camera(app_state *state) {
  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->window_area.dpi_scaling;
  area /= (state->game.camera.zoom * 100.f);

  state->game.renderer.camp = glm::ortho(-area.x, area.x, -area.y, area.y, -1.f, 1.f);
  state->game.renderer.camv = glm::translate(glm::mat4(1.f), glm::vec3(state->game.camera.pos, 0.f));
}

void render_game(app_state *state) {
  render_calculate_camera(state);
  render_pass_geometry(state);
  render_pass_bloom(state);
  render_pass_tonemap(state);
}
