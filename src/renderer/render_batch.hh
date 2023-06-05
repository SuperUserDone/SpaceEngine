#pragma once

#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "data/game_state.hh"
#include "data/glm_exts.hh"
#include "pyrolib/container/arena_vector.hh"
#include "renderer/pipeline_settings.hh"

struct render_batch {
  renderer_mesh mesh;

  pyro::container::arena_vector<vertex> verticies;
  pyro::container::arena_vector<uint32_t> indicies;
};

APIFUNC render_batch render_batch_create(app_state *state, size_t max_batch_size = 2048);
APIFUNC void render_batch_reset(render_batch &batch);
APIFUNC void render_batch_add_rect(render_batch &batch, rect &r);
APIFUNC void render_batch_render(app_state *state,
                                 render_batch &batch,
                                 renderer_pipeline &pipeline,
                                 pipeline_settings &settings);