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

  data.indicies.lt_ref(m_indicies);
  data.verticies.lt_ref(m_verticies);

  m_mesh = state->api.renderer.create_mesh(&data);
  m_state = state;
  m_data_hash = 0;
}

void batch::clear() {
  ZoneScopedN("Batch clear");
  m_indicies.clear();
  m_verticies.clear();
}

void batch::add_rect(pyro::math::quad &r) {
  ZoneScopedN("Batch add mesh");

  size_t start = m_verticies.size();
  {
    ZoneScopedN("Batch add mesh verticies");

    m_verticies.insert_back(r.begin(), r.end());
  }
  {
    ZoneScopedN("Batch add mesh indicies");
    m_indicies.push_back(start + 0);
    m_indicies.push_back(start + 1);
    m_indicies.push_back(start + 2);
    m_indicies.push_back(start + 0);
    m_indicies.push_back(start + 2);
    m_indicies.push_back(start + 3);
  }
}
void batch::update_mesh() {
  ZoneScopedN("Batch update mesh");

  {
    ZoneScopedN("Compute hash");
    size_t vhash =
        XXH3_64bits(&m_verticies[0], sizeof(decltype(m_verticies)::type) * m_verticies.size());
    size_t ihash =
        XXH3_64bits(&m_indicies[0], sizeof(decltype(m_indicies)::type) * m_indicies.size());
    size_t hash = vhash ^ ihash;

    if (m_data_hash == hash)
      return;

    m_data_hash = hash;
  }

  mesh_data data;

  data.indicies.lt_ref(m_indicies);
  data.verticies.lt_ref(m_verticies);

  m_state->api.renderer.update_mesh(&m_mesh, &data);
}

void batch::render(renderer_pipeline &pipeline, pipeline_settings &settings) {
  ZoneScopedN("Batch render");
  update_mesh();

  renderer_mesh *m = &m_mesh;
  pipeline_settings *pp = &settings;
  m_state->api.renderer.draw_meshes(1, &m, &pp, &pipeline);
}


void batch::reserve_rects(size_t count) {
  m_indicies.reserve(count * 6);
  m_verticies.reserve(count * 4);
}

void batch::lt_done() {
  ZoneScopedN("Batch delete");
  m_state->api.renderer.delete_mesh(m_mesh);
  m_indicies.lt_done();
  m_verticies.lt_done();
}

} // namespace render
