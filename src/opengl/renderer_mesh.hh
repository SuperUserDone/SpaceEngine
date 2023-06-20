#pragma once

#include "data/asset_types.hh"
#include "glad/gl.h"
#include "opengl/renderer_state.hh"
#include "renderer_state.hh"
#include "tracy/Tracy.hpp"

static inline void update_mesh(renderer_mesh *r, mesh_data *data) {
  ZoneScopedN("Update GPU Mesh");
  internal_mesh *m = reinterpret_cast<internal_mesh *>(r->index);

  r->triangle_vertex_count = data->index_count;

  if (data->index_count == 0 && data->vertex_count == 0)
    return;

  // Fill Buffers
  {
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
    glBufferData(GL_ARRAY_BUFFER,
                 data->vertex_count * sizeof(vertex),
                 data->verticies,
                 GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data->index_count * sizeof(uint32_t),
                 data->indicies,
                 GL_STREAM_DRAW);
  }

  // Setup vertex attribs
  {
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, uv));
  }


}

static inline renderer_mesh create_mesh(mesh_data *data) {
  ZoneScopedN("Create GPU Mesh");
  internal_mesh *m = rstate->internal_mesh_data.alloc();

  // Create buffers
  {
    glGenBuffers(1, &m->vb);
    glGenBuffers(1, &m->ib);

    glGenVertexArrays(1, &m->vao);
  }
  renderer_mesh mesh;
  mesh.index = reinterpret_cast<size_t>(m);

  update_mesh(&mesh, data);

  return mesh;
}

static inline void delete_mesh(renderer_mesh r) {
  ZoneScopedN("Delete GPU Mesh");

  internal_mesh *m = reinterpret_cast<internal_mesh *>(r.index);
  glDeleteVertexArrays(1, &m->vao);
  glDeleteBuffers(1, &m->ib);
  glDeleteBuffers(1, &m->vb);
}
