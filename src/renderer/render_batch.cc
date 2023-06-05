#include "render_batch.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "renderer/render_batch.hh"

render_batch render_batch_create(app_state *state, size_t max_batch_size) {
  render_batch b;

  b.verticies.lt_init(max_batch_size * 4);
  b.indicies.lt_init(max_batch_size * 6);

  mesh_data data;
  data.index_count = b.indicies.size();
  data.vertex_count = b.verticies.size();

  data.indicies = &b.indicies[0];
  data.verticies = &b.verticies[0];

  b.mesh = state->api.renderer.create_mesh(&data);

  return b;
}

void render_batch_reset(render_batch &batch) {
  batch.indicies.clear();
  batch.verticies.clear();
}

void render_batch_add_rect(render_batch &batch, rect &r) {
  // Don't draw if size == 0
  if (r.size == glm::vec2(0)) {
    return;
  }

  size_t start = batch.verticies.size();

  batch.verticies.push_back(vertex{r.pos, r.uva});
  batch.verticies.push_back(
      vertex{glm::vec2(r.pos.x, r.pos.y + r.size.y), glm::vec2(r.uva.x, r.uvb.y)});
  batch.verticies.push_back(vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y + r.size.y), r.uvb});
  batch.verticies.push_back(
      vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y), glm::vec2{r.uvb.x, r.uva.y}});

  batch.indicies.push_back(start + 0);
  batch.indicies.push_back(start + 1);
  batch.indicies.push_back(start + 2);
  batch.indicies.push_back(start + 0);
  batch.indicies.push_back(start + 2);
  batch.indicies.push_back(start + 3);
}

void update_gpu_mesh(app_state *state, render_batch &batch) {
  mesh_data data;
  data.index_count = batch.indicies.size();
  data.vertex_count = batch.verticies.size();

  data.indicies = &batch.indicies[0];
  data.verticies = &batch.verticies[0];

  state->api.renderer.update_mesh(&batch.mesh, &data);
}

void render_batch_render(app_state *state,
                         render_batch &batch,
                         renderer_pipeline &pipeline,
                         pipeline_settings &settings) {
  update_gpu_mesh(state, batch);

  renderer_mesh *m = &batch.mesh;
  pipeline_settings *pp = &settings;
  state->api.renderer.draw_meshes(1, &m, &pp, &pipeline);
}