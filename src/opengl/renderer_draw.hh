#pragma once

#include "common/memory_pool.hh"
#include "data/asset_types.hh"
#include "data/renderer_api.hh"
#include "opengl/renderer_state.hh"
#include "renderer_pipeline.hh"

inline static void bind_pipeline(renderer_pipeline *pipeline) {
  glUseProgram(pipeline->index);
}

inline static void bind_settings(renderer_pipeline *p, pipeline_settings *s) {
  for (size_t i = 0; i < s->uniform_count; i++) {
    switch (s->uniforms[0].type) {
    case UNIFORM_TYPE_SCALAR:
      glUniform1f(p->uniform_indicies[s->uniforms[0].index], s->uniforms[0].scalar);
      break;
    case UNIFORM_TYPE_TEXTURE:
      break;
    case UNIFORM_TYPE_VEC2:
      break;
    case UNIFORM_TYPE_VEC3:
      break;
    case UNIFORM_TYPE_VEC4:
      break;
    case UNIFORM_TYPE_MAT4:
      break;
    }
  }
}

inline static void draw_mesh(renderer_mesh *mesh) {
  internal_mesh *m = pool_get_at_index(rstate->internal_mesh_data, mesh->index);
  glBindVertexArray(m->vao);
  // glBindBuffer(GL_ARRAY_BUFFER, m->vb); 
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib); 

  glDrawElements(GL_TRIANGLES, mesh->triangle_vertex_count, GL_UNSIGNED_INT, 0);
}

inline static void render_meshes(size_t count,
                                 renderer_mesh **mesh,
                                 pipeline_settings **settings,
                                 renderer_pipeline *pipeline) {
  bind_pipeline(pipeline);

  for (size_t i = 0; i < count; i++) {
    bind_settings(pipeline, settings[i]);
    draw_mesh(mesh[i]);
  }
}
