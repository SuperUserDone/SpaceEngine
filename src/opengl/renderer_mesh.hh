#pragma once
#include "common/memory_pool.hh"
#include "data/asset_types.hh"
#include "glad/gl.h"
#include "renderer_state.hh"

static inline void update_mesh(renderer_mesh *r, mesh_data *data) {
  internal_mesh *m = pool_get_at_index(rstate->internal_mesh_data, r->index);

  // Fill Buffers
  {
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
    glBufferData(GL_ARRAY_BUFFER,
                 data->vertex_count * sizeof(glm::vec2),
                 data->verticies,
                 GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data->index_count * sizeof(uint32_t),
                 data->indicies,
                 GL_STREAM_DRAW);
  }
}

static inline renderer_mesh create_mesh(mesh_data *data) {
  internal_mesh *m = pool_alloc(rstate->internal_mesh_data);

  // Create buffers
  {
    glGenBuffers(1, &m->vb);
    glGenBuffers(1, &m->ib);

    glGenVertexArrays(1, &m->vao);
  }
  renderer_mesh mesh{(uint32_t)pool_get_index(rstate->internal_mesh_data, m)};

  update_mesh(&mesh, data);

  // Setup vertex attribs
  {
    glBindBuffer(GL_ARRAY_BUFFER, m->ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
  }

  return mesh;
}

static inline void delete_mesh(renderer_mesh r) {
  internal_mesh *m = pool_get_at_index(rstate->internal_mesh_data, r.index);
  glDeleteVertexArrays(1, &m->vao);
  glDeleteBuffers(1, &m->ib);
  glDeleteBuffers(1, &m->vb);
}

