#include "data/renderer_api.hh"

#include "common/win32_export.hh"

#include "glad/gl.c"

bool renderer_init(load_proc proc) {
  gladLoadGL(proc);

  const unsigned char *name = glGetString(GL_VENDOR);
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
APIFUNC renderer_api fetch_api() {
  renderer_api api;

  api.init = renderer_init;
  api.shutdown = renderer_shutdown;
  api.set_viewport = renderer_set_viewport;
  api.clear = renderer_clear;

  return api;
}
}