#include "backends/imgui_impl_opengl3.h"
#include "common/memory_arena.hh"
#include "common/memory_pool.hh"
#include "data/app_state.hh"

#include "common/win32_export.hh"

#include "glad/gl.c"
#include "renderer_draw.hh"
#include "renderer_mesh.hh"
#include "renderer_pipeline.hh"
#include "renderer_state.hh"
#include "renderer_texture.hh"

#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "tracy/Tracy.hpp"

static renderer_state *rstate;

bool renderer_init(mem_arena &scratch, app_state *state, load_proc proc) {
  gladLoadGL(proc);
  glEnable(GL_FRAMEBUFFER_SRGB);

  if (!state->renderer_state) {
    rstate = arena_push_struct(state->permanent_arena, renderer_state);
    rstate->internal_mesh_data = pool_create<internal_mesh>(65536);
    state->renderer_state = rstate;
    rstate->perm_data = arena_create();
    ImGui_ImplOpenGL3_Init("#version 330 core");
  } else {
    rstate = (renderer_state *)state->renderer_state;
  }
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
  for (int i = 0; i < 0; i++) {
    glDeleteProgram(i);
  }
  return true;
}

void imgui_begin() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void imgui_end() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

extern "C" {
ALWAYS_EXPORT void fetch_api(app_state *state) {
  state->api.renderer.init = renderer_init;
  state->api.renderer.shutdown = renderer_shutdown;
  state->api.renderer.set_viewport = renderer_set_viewport;
  state->api.renderer.clear = renderer_clear;

  state->api.renderer.create_pipeline = create_pipeline;
  state->api.renderer.delete_pipeline = delete_pipeline;

  state->api.renderer.create_texture = create_texture;
  state->api.renderer.update_texture = update_texture;
  state->api.renderer.delete_texture = delete_texture;

  state->api.renderer.create_mesh = create_mesh;
  state->api.renderer.update_mesh = update_mesh;
  state->api.renderer.delete_mesh = delete_mesh;

  state->api.renderer.draw_meshes = render_meshes;

  state->api.renderer.imgui_begin = imgui_begin;
  state->api.renderer.imgui_end = imgui_end;
}
}
