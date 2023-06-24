#pragma once

#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/game_state.hh"
#include "data/glm_exts.hh"
#include "pyrolib/container/arena_vector.hh"
#include "renderer/pipeline_settings.hh"

namespace render {

class APIFUNC batch {
public:
  void lt_init(app_state *state, size_t max_verts = 8192);
  void lt_done();

  void clear();

  void render(renderer_pipeline &pipeline, pipeline_settings &settings);
  void add_rect(rect &rect);

private:
  renderer_mesh m_mesh;

  pyro::container::arena_vector<vertex> m_verticies;
  pyro::container::arena_vector<uint32_t> m_indicies;

  app_state *m_state;

  size_t m_data_hash; 

  void update_mesh();
};

} // namespace render
