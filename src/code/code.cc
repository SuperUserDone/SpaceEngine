#include "common/memory_arena.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "common/file_utils.hh"
#include "data/asset_types.hh"
#include <math.h>
#include <stdlib.h>

void init(app_state *state) {
  char * frag = load_file(state->frame_arena, "data/shaders/opaque_color.frag.glsl");
  char * vert = load_file(state->frame_arena, "data/shaders/opaque_color.vert.glsl");

  pipeline_data d;
  d.vertex_shader = vert;
  d.fragment_shader = frag;
  d.uniform_count = 1;
  const char *data = "color";
  d.uniform_names = &data;
}

void update(app_state *state) {
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
