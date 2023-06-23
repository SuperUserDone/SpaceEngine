#include "render_batch.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "renderer/render_batch.hh"

namespace render {
void batch::lt_init(app_state *state, size_t max_verts) {

  m_verticies.lt_init(max_verts * 4);
  m_indicies.lt_init(max_verts * 6);

  mesh_data data;
  data.index_count = m_indicies.size();
  data.vertex_count = m_verticies.size();

  data.indicies = &m_indicies[0];
  data.verticies = &m_verticies[0];

  m_mesh = state->api.renderer.create_mesh(&data);
  m_state = state;
}

void batch::clear() {
  m_indicies.clear();
  m_verticies.clear();
}

void batch::add_rect(rect &r) {
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
  mesh_data data;
  data.index_count = m_indicies.size();
  data.vertex_count = m_verticies.size();

  data.indicies = &m_indicies[0];
  data.verticies = &m_verticies[0];

  m_state->api.renderer.update_mesh(&m_mesh, &data);
}

void batch::render(renderer_pipeline &pipeline, pipeline_settings &settings) {
  update_mesh();

  renderer_mesh *m = &m_mesh;
  pipeline_settings *pp = &settings;
  m_state->api.renderer.draw_meshes(1, &m, &pp, &pipeline);
}

void batch::lt_done() {
  m_state->api.renderer.delete_mesh(m_mesh);
  m_indicies.lt_done();
  m_verticies.lt_done();
}

} // namespace render
