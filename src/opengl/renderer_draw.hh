#pragma once

#include "memory/memory_pool.hh"
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
  for (size_t i = 0; i < s->uniform_count; i++) {
    switch (s->uniforms[i].type) {
    case UNIFORM_TYPE_SCALAR:
      glUniform1f(p->uniform_indicies[s->uniforms[i].index], s->uniforms[i].scalar);
      break;
    case UNIFORM_TYPE_TEXTURE:
      glActiveTexture(GL_TEXTURE0 + bound_tex);
      glBindTexture(GL_TEXTURE_2D, s->uniforms[i].texture.index);
      glUniform1i(p->uniform_indicies[s->uniforms[i].index], bound_tex++);
      break;
    case UNIFORM_TYPE_VEC2:
      glUniform2f(p->uniform_indicies[s->uniforms[i].index],
                  s->uniforms[i].vec2.x,
                  s->uniforms[i].vec2.y);
      break;
    case UNIFORM_TYPE_VEC3:
      glUniform3f(p->uniform_indicies[s->uniforms[i].index],
                  s->uniforms[i].vec3.x,
                  s->uniforms[i].vec3.y,
                  s->uniforms[i].vec3.z);
      break;
    case UNIFORM_TYPE_VEC4:
      glUniform4f(p->uniform_indicies[s->uniforms[i].index],
                  s->uniforms[i].vec4.x,
                  s->uniforms[i].vec4.y,
                  s->uniforms[i].vec4.z,
                  s->uniforms[i].vec4.w);
      break;
    case UNIFORM_TYPE_MAT4:
      glUniformMatrix4fv(p->uniform_indicies[s->uniforms[i].index],
                         1,
                         GL_FALSE,
                         (float *)&s->uniforms[i].mat4);
      break;
    case UNIFORM_TYPE_INTEGER:
      glUniform1i(p->uniform_indicies[s->uniforms[i].index], s->uniforms[i].integer);
      break;
    }
  }
}

inline static void draw_mesh(renderer_mesh *mesh) {
  ZoneScopedN("Drawcall mesh");

  internal_mesh *m = pool_get_at_index(rstate->internal_mesh_data, mesh->index);
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
