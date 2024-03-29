#include "backends/imgui_impl_opengl3.h"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/renderer_api.hh"
#include "glad/gl.c"
#include "pyrolib/memory/arena.hh"
#include "tracy/Tracy.hpp"

#include "renderer_draw.hh"
#include "renderer_framebuffer.hh"
#include "renderer_mesh.hh"
#include "renderer_pipeline.hh"
#include "renderer_state.hh"
#include "renderer_texture.hh"

#include <imgui.h>

static renderer_state *rstate;

bool renderer_init(app_state *state, load_proc proc) {
  gladLoadGL(proc);
  glEnable(GL_FRAMEBUFFER_SRGB);

  if (!state->renderer_state) {
    rstate = state->permanent_arena.push<renderer_state>();
    state->renderer_state = rstate;
  } else {
    rstate = (renderer_state *)state->renderer_state;
  }

  rstate->perm_data.lt_init();
  rstate->internal_mesh_data.lt_init(65536);
  ImGui_ImplOpenGL3_Init("#version 330 core");

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
  rstate->internal_mesh_data.lt_done();
  rstate->perm_data.lt_done();
  rstate = nullptr;
  ImGui_ImplOpenGL3_Shutdown();
  return true;
}

void renderer_set_blending(blending_func f) {
  glEnable(GL_BLEND);
  switch (f) {
  case BLEND_ADD:
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    break;
  case BLEND_ONE_MIN_SRC_ALPHA:
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  }
}

void imgui_begin() {
  ZoneScopedN("ImguiBegin");
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void imgui_end() {
  ZoneScopedN("ImguiEnd");
  {
    ZoneScopedN("ImguiRender");
    ImGui::Render();
  }
  {
    ZoneScopedN("RenderDrawData");
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
}

size_t get_max_texture_size() {
  GLint ts;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &ts);

  return ts;
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.renderer.init = renderer_init;
  state->api.renderer.shutdown = renderer_shutdown;
  state->api.renderer.set_viewport = renderer_set_viewport;
  state->api.renderer.clear = renderer_clear;

  state->api.renderer.create_framebuffer = create_framebuffer;
  state->api.renderer.delete_framebuffer = delete_framebuffer;
  state->api.renderer.use_framebuffer = use_framebuffer;
  state->api.renderer.use_default_framebuffer = use_default_framebuffer;

  state->api.renderer.create_pipeline = create_pipeline;
  state->api.renderer.delete_pipeline = delete_pipeline;

  state->api.renderer.create_texture = create_texture;
  state->api.renderer.update_texture = update_texture;
  state->api.renderer.delete_texture = delete_texture;
  state->api.renderer.get_max_texture_size = get_max_texture_size;

  state->api.renderer.create_mesh = create_mesh;
  state->api.renderer.update_mesh = update_mesh;
  state->api.renderer.delete_mesh = delete_mesh;

  state->api.renderer.draw_meshes = render_meshes;

  state->api.renderer.imgui_begin = imgui_begin;
  state->api.renderer.imgui_end = imgui_end;

  state->api.renderer.set_blending = renderer_set_blending;
}
}
