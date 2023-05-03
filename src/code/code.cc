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
#include "renderer/renderer.hh"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void load_assets(app_state *state) {
  char *tonemap_frag = load_file(state->frame_arena, "data/shaders/tonemap.frag.glsl");
  char *default_vert = load_file(state->frame_arena, "data/shaders/default.vert.glsl");

  texture_data td;

  int c;
  int x, y;
  td.data = stbi_load("data/textures/organic.jpg", &x, &y, &c, 3);
  td.format = TEX_FORMAT_SRGB;
  td.w = x;
  td.h = y;

  asset_texture_create(state, HASH_KEY("organic1"), &td);
  stbi_image_free(td.data);

  pipeline_data d;
  d.vertex_shader = default_vert;
  d.fragment_shader = tonemap_frag;
  d.uniform_count = 2;
  const char *names[] = {"transform", "quad"};
  d.uniform_names = names;

  asset_pipeline_create(state, HASH_KEY("Tonemap"), &d);

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
  state->game.sun_color = {231.f / 255.f, 103.f / 255.f, 16.f / 255.f};
  render_init(state);
}

void render(app_state *state) {
  render_game(state);
}

void tick(app_state *state) {
  // TODO
}

void shutdown(app_state *state) {
  // TODO
}

void draw_debug_info(app_state *state) {
  ImGui::Begin("Camera");
  ImGui::DragFloat("Zoom", &state->game.camera.zoom, 0.1f, 0.01f, 100.f);
  ImGui::DragFloat2("Pos", (float *)&state->game.camera.pos);
  ImGui::ColorEdit3("SunColor", (float *)&state->game.sun_color);
  ImGui::End();
}

void process_event(app_state *state, event &e) {
  switch (e.type) {
  case EVENT_TYPE_NONE:
    break;
  case EVENT_TYPE_QUIT:
    state->running = false;
    break;
  case EVENT_TYPE_RESIZE:
    render_resize(state, e.data.resize.newx, e.data.resize.newy);
    break;
  }
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.render = render;
  state->api.game.tick = tick;
  state->api.game.shutdown = shutdown;
  state->api.game.event = process_event;

  state->api.game.draw_debug_info = draw_debug_info;
  state->api.game.load_assets = load_assets;
}
}
