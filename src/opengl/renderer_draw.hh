#pragma once

#include "data/asset_types.hh"
#include "data/renderer_api.hh"
#include "opengl/renderer_state.hh"
#include "renderer_pipeline.hh"
#include "tracy/Tracy.hpp"

inline static void bind_pipeline(renderer_pipeline *pipeline) {
  ZoneScopedN("GPU Bind Pipeline");
  glUseProgram(pipeline->index);
}

inline static void bind_settings(renderer_pipeline *p, pipeline_settings *s) {
  ZoneScopedN("GPU Bind Pipeline Settings");
  int bound_tex = 0;
  for (auto &uniform : s->uniforms) {
    switch (uniform.type) {
    case UNIFORM_TYPE_SCALAR:
      glUniform1f(p->uniform_indicies[uniform.index], uniform.scalar);
      break;
    case UNIFORM_TYPE_TEXTURE:
      glActiveTexture(GL_TEXTURE0 + bound_tex);
      glBindTexture(GL_TEXTURE_2D, uniform.texture.index);
      glUniform1i(p->uniform_indicies[uniform.index], bound_tex++);
      break;
    case UNIFORM_TYPE_VEC2:
      glUniform2f(p->uniform_indicies[uniform.index], uniform.vec2.x, uniform.vec2.y);
      break;
    case UNIFORM_TYPE_VEC3:
      glUniform3f(p->uniform_indicies[uniform.index],
                  uniform.vec3.x,
                  uniform.vec3.y,
                  uniform.vec3.z);
      break;
    case UNIFORM_TYPE_VEC4:
      glUniform4f(p->uniform_indicies[uniform.index],
                  uniform.vec4.x,
                  uniform.vec4.y,
                  uniform.vec4.z,
                  uniform.vec4.w);
      break;
    case UNIFORM_TYPE_MAT4:
      glUniformMatrix4fv(p->uniform_indicies[uniform.index], 1, GL_FALSE, (float *)&uniform.mat4);
      break;
    case UNIFORM_TYPE_INTEGER:
      glUniform1i(p->uniform_indicies[uniform.index], uniform.integer);
      break;
    }
  }
}

inline static void draw_mesh(renderer_mesh *mesh) {
  ZoneScopedN("Drawcall mesh");

  internal_mesh *m = reinterpret_cast<internal_mesh *>(mesh->index);
  glBindVertexArray(m->vao);

  glDrawElements(GL_TRIANGLES, mesh->triangle_vertex_count, GL_UNSIGNED_INT, 0);
}

inline static void render_meshes(size_t count,
                                 renderer_mesh **mesh,
                                 pipeline_settings **settings,
                                 renderer_pipeline *pipeline) {
  ZoneScopedN("Draw Meshes");
  bind_pipeline(pipeline);

  for (size_t i = 0; i < count; i++) {
    bind_settings(pipeline, settings[i]);
    draw_mesh(mesh[i]);
  }
}
