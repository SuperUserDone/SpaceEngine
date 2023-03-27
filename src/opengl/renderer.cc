#include "data/app_state.hh"
#include "data/renderer_api.hh"

#include "common/win32_export.hh"

#include "glad/gl.c"

bool renderer_init(load_proc proc) {
  gladLoadGL(proc);
  return true;
}

void renderer_set_viewport(int x, int y) {
  glViewport(0, 0, x, y);
}

void renderer_clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

bool renderer_shutdown() {
  return true;
}

extern "C" {
APIFUNC void fetch_api(app_state *state) {
  state->api.renderer.init = renderer_init;
  state->api.renderer.shutdown = renderer_shutdown;
  state->api.renderer.set_viewport = renderer_set_viewport;
  state->api.renderer.clear = renderer_clear;
}
}