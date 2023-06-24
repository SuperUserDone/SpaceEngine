#include "render_batch.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "renderer/render_batch.hh"
#include "tracy/Tracy.hpp"

namespace render {
void batch::lt_init(app_state *state, size_t max_verts) {
  ZoneScopedN("Batch init");
  m_verticies.lt_init(max_verts * 4);
  m_indicies.lt_init(max_verts * 6);

  mesh_data data;
  data.index_count = m_indicies.size();
  data.vertex_count = m_verticies.size();

  data.indicies = &m_indicies[0];
  data.verticies = &m_verticies[0];

  m_mesh = state->api.renderer.create_mesh(&data);
  m_state = state;
  m_data_hash = 0;
}

void batch::clear() {
  ZoneScopedN("Batch clear");
  m_indicies.clear();
  m_verticies.clear();
}

void batch::add_rect(rect &r) {
  ZoneScopedN("Batch add mesh");
  // Don't draw if size == 0
  if (r.size == glm::vec2(0)) {
    return;
  }

  size_t start = m_verticies.size();

  m_verticies.push_back(vertex{r.pos, r.uva});
  m_verticies.push_back(
      vertex{glm::vec2(r.pos.x, r.pos.y + r.size.y), glm::vec2(r.uva.x, r.uvb.y)});
  m_verticies.push_back(vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y + r.size.y), r.uvb});
  m_verticies.push_back(
      vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y), glm::vec2{r.uvb.x, r.uva.y}});

  m_indicies.push_back(start + 0);
  m_indicies.push_back(start + 1);
  m_indicies.push_back(start + 2);
  m_indicies.push_back(start + 0);
  m_indicies.push_back(start + 2);
  m_indicies.push_back(start + 3);
}
void batch::update_mesh() {
  ZoneScopedN("Batch update mesh");

  {
    ZoneScopedN("Compute hash");
    size_t vhash = XXH3_64bits(&m_verticies[0], sizeof(vertex) * m_verticies.size());
    size_t ihash = XXH3_64bits(&m_indicies[0], sizeof(uint32_t) * m_indicies.size());
    size_t hash = vhash ^ ihash;

    if (m_data_hash == hash)
      return;

    m_data_hash = hash;
  }

  mesh_data data;
  data.index_count = m_indicies.size();
  data.vertex_count = m_verticies.size();

  data.indicies = &m_indicies[0];
  data.verticies = &m_verticies[0];

  m_state->api.renderer.update_mesh(&m_mesh, &data);
}

void batch::render(renderer_pipeline &pipeline, pipeline_settings &settings) {
  ZoneScopedN("Batch render");
  update_mesh();

  renderer_mesh *m = &m_mesh;
  pipeline_settings *pp = &settings;
  m_state->api.renderer.draw_meshes(1, &m, &pp, &pipeline);
}

void batch::lt_done() {
  ZoneScopedN("Batch delete");
  m_state->api.renderer.delete_mesh(m_mesh);
  m_indicies.lt_done();
  m_verticies.lt_done();
}

} // namespace render
