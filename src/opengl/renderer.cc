#include "data/app_state.hh"

#include "common/win32_export.hh"

#include "glad/gl.c"
#include "opengl/renderer_state.hh"

#include "renderer_shaders.hh"

static renderer_state ren_state;

bool renderer_init(mem_arena &scratch, app_state *state, load_proc proc) {
  gladLoadGL(proc);

  ren_state.shaders[OPAQUE_COLOR] = compile_shader(scratch, "opaque_color", state);

  ren_state.opaque_shader_data.color_pos =
      glGetUniformLocation(ren_state.shaders[OPAQUE_COLOR], "color");

  return true;
}

void renderer_set_viewport(int x, int y) {
  glViewport(0, 0, x, y);
}

void renderer_clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_render_mesh(app_state *state, mesh &mesh_data, mesh_render_material &mat) {
  // Alloc buffers
  GLuint vertex_buffer;
  GLuint index_buffer;
  GLuint VAO;

  // Create buffers
  {
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
  }

  // Fill Buffers
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh_data.vertex_count * sizeof(mesh_vertex),
                 mesh_data.verticies,
                 GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh_data.index_count * sizeof(uint32_t),
                 mesh_data.indicies,
                 GL_STREAM_DRAW);
  }

  // Bind shader
  {
    glUseProgram(ren_state.shaders[mat.type]);
    // Shader Attrs
    switch (mat.type) {

    case OPAQUE_COLOR:
      glUniform3f(ren_state.opaque_shader_data.color_pos,
                  mat.opaque_color.r,
                  mat.opaque_color.g,
                  mat.opaque_color.b);
      break;
    case COUNT:
      break;
    }
  }

  // Setup vertex attribs
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), nullptr);
  }
  // Draw
  { glDrawElements(GL_TRIANGLES, mesh_data.index_count, GL_UNSIGNED_INT, 0); }

  // Cleanup
  {
    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);
    glDeleteVertexArrays(1, &VAO);
  }
}

bool renderer_shutdown() {
  for (int i = 0; i < mesh_render_material_type::OPAQUE_COLOR; i++) {
    glDeleteProgram(i);
  }
  return true;
}

extern "C" {
APIFUNC void fetch_api(app_state *state) {
  state->api.renderer.init = renderer_init;
  state->api.renderer.shutdown = renderer_shutdown;
  state->api.renderer.set_viewport = renderer_set_viewport;
  state->api.renderer.clear = renderer_clear;
  state->api.renderer.render_mesh = renderer_render_mesh;
}
}