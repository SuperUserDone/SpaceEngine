#include "assetmanager/assetmanager.hh"
#include "common/file_utils.hh"
#include "common/memory_arena.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/renderer_api.hh"
#include <math.h>
#include <stdlib.h>

void init(app_state *state) {
  char *frag = load_file(state->frame_arena, "data/shaders/opaque_color.frag.glsl");
  char *vert = load_file(state->frame_arena, "data/shaders/opaque_color.vert.glsl");

  pipeline_data d;
  d.vertex_shader = vert;
  d.fragment_shader = frag;
  d.uniform_count = 2;
  const char *names[] = {"color", "tex"};
  d.uniform_names = names;

  uint32_t b = asset_bundle_create(state, "code");
  uint32_t pipeline_id = asset_pipeline_create(state, b, &d);

  mesh_data mesh;
  vertex verts[] = {{{1.f, 1.f}, {1.f, 1.f}}, {{1.f, 0.f}, {1.f, 0.f}}, {{0.f, 0.f}, {0.f, 0.f}}};
  uint32_t indicies[] = {0, 1, 2};
  mesh.verticies = verts;
  mesh.indicies = indicies;
  mesh.vertex_count = 3;
  mesh.index_count = 3;

  uint32_t mesh_id = asset_mesh_create(state, b, &mesh);

  texture_data tex;
  tex.w = 4;
  tex.h = 4;
  tex.data = nullptr;

  uint32_t t = asset_texture_create(state, b, &tex);

  state->game.m = mesh_id;
  state->game.p = pipeline_id;
  state->game.b = b;
  state->game.tex = t;
}

void update(app_state *state) {
  pipeline_settings s;

  renderer_uniform u[2];
  u[0].type = UNIFORM_TYPE_VEC3;
  u[0].vec3 = {0.3f, 0.7f, 0.4f};
  u[0].index = 0;
  u[1].type = UNIFORM_TYPE_TEXTURE;
  u[1].texture = asset_texture_get_render(state, state->game.b, state->game.tex);
  u[1].index = 1;

  renderer_pipeline p = asset_pipeline_get_render(state, state->game.b, state->game.p);
  renderer_mesh m = asset_mesh_get_render(state, state->game.b, state->game.m);
  s.uniforms = u;
  s.uniform_count = 2;

  renderer_mesh *mp = &m;
  pipeline_settings *pp = &s;

  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

void shutdown(app_state *state) {
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.update = update;
  state->api.game.shutdown = shutdown;
}
}
