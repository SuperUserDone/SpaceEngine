#include "data/app_state.hh"
#include <math.h>
#include <stdlib.h>
void init(mem_arena &permanent_arena, app_state *state) {
}

void update(mem_arena &permanent_arena, mem_arena &frame_arena, app_state *state) {
  mesh m;
  mesh_render_material mat;
  mat.type = OPAQUE_COLOR;
  mat.opaque_color = {0, 0, 1};

  mesh_vertex v[] = {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};
  m.verticies = v;
  m.vertex_count = 3;

  uint32_t i[] = {0, 1, 2};
  m.indicies = i;
  m.index_count = 3;

  state->api.renderer.render_mesh(state, m, mat);
}

void shutdown(mem_arena &permanent_arena, app_state *state) {
}

extern "C" {
APIFUNC void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.update = update;
  state->api.game.shutdown = shutdown;
}
}