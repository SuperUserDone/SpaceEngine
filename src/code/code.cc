#include "assetmanager/assetmanager.hh"
#include "common/file_utils.hh"
#include "common/hash.hh"
#include "common/hash_table.hh"
#include "common/memory_arena.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/renderer_api.hh"
#include "imgui.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void load_assets(app_state *state) {
  char *sol_frag = load_file(state->frame_arena, "data/shaders/sol.frag.glsl");
  char *default_vert = load_file(state->frame_arena, "data/shaders/default.vert.glsl");

  texture_data td;

  int c;
  int x, y;
  td.data = stbi_load("data/textures/organic.jpg", &x, &y, &c, 3);

  td.w = x;
  td.h = y;

  asset_texture_create(state, HASH_KEY("organic1"), &td);

  pipeline_data d;
  d.vertex_shader = default_vert;
  d.fragment_shader = sol_frag;
  d.uniform_count = 2;
  const char *names[] = {"time", "transform", "organic"};
  d.uniform_names = names;

  asset_pipeline_create(state, HASH_KEY("pipeline"), &d);

  mesh_data mesh;
  vertex verts[] = {{{100.f, 100.f}, {1.f, 1.f}},
                    {{100.f, -100.f}, {1.f, 0.f}},
                    {{-100.f, -100.f}, {0.f, 0.f}},
                    {{-100.f, 100.f}, {0.f, 1.f}}};
  uint32_t indicies[] = {0, 1, 2, 0, 2, 3};
  mesh.verticies = verts;
  mesh.indicies = indicies;
  mesh.vertex_count = 4;
  mesh.index_count = 6;

  asset_mesh_create(state, HASH_KEY("mesh"), &mesh);
}

void init(app_state *state) {
  state->game.camera.pos = {0, 0};
  state->game.camera.zoom = 9.f;
}

void update(app_state *state) {
  pipeline_settings s;

  glm::vec2 area = {(float)state->window_area.w, (float)state->window_area.h};
  area /= state->game.camera.zoom;

  glm::mat4 cam = glm::ortho(-area.x, area.x, -area.y, area.y, -1.f, 1.f) *
                  glm::translate(glm::mat4(1.f), glm::vec3(-state->game.camera.pos, 0.f));

  renderer_uniform u[3];
  u[0].type = UNIFORM_TYPE_SCALAR;
  u[0].scalar = state->time.t;
  u[0].index = 0;
  u[1].type = UNIFORM_TYPE_MAT4;
  u[1].mat4 = cam;
  u[1].index = 1;
  u[2].type = UNIFORM_TYPE_TEXTURE;
  u[2].texture = asset_texture_get_render(state, HASH_KEY("organic1"));
  u[2].index = 2;

  renderer_pipeline p = asset_pipeline_get_render(state, HASH_KEY("pipeline"));
  renderer_mesh m = asset_mesh_get_render(state, HASH_KEY("mesh"));
  s.uniforms = u;
  s.uniform_count = 2;

  renderer_mesh *mp = &m;
  pipeline_settings *pp = &s;

  state->api.renderer.draw_meshes(1, &mp, &pp, &p);
}

void shutdown(app_state *state) {
}

void draw_debug_info(app_state *state) {
  ImGui::Begin("Camera");
  ImGui::DragFloat("Zoom", &state->game.camera.zoom, 0.1f, 0.01f, 100.f);
  ImGui::DragFloat2("Pos", (float *)&state->game.camera.pos);
  ImGui::End();
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.update = update;
  state->api.game.shutdown = shutdown;

  state->api.game.draw_debug_info = draw_debug_info;
  state->api.game.load_assets = load_assets;
}
}
