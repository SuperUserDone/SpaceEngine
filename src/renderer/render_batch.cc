#include "render_batch.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "memory/memory_arena_typed.hh"
#include "renderer/render_batch.hh"

render_batch render_batch_create(app_state *state, size_t max_batch_size) {
  render_batch b;

  b.verticies_arena = arena_typed_create<vertex>(max_batch_size * 4);
  b.indicies_arena = arena_typed_create<uint32_t>(max_batch_size * 6);

  b.data.verticies = (vertex *)b.verticies_arena.arena.base;
  b.data.indicies = (uint32_t *)b.indicies_arena.arena.base;
  b.data.vertex_count = 0;
  b.data.index_count = 0;

  b.mesh = state->api.renderer.create_mesh(&b.data);

  return b;
}

void render_batch_reset(render_batch &batch) {
  batch.data.index_count = 0;
  batch.data.vertex_count = 0;

  arena_typed_clear(batch.indicies_arena);
  arena_typed_clear(batch.verticies_arena);
}

void render_batch_add_rect(render_batch &batch, rect &r) {
  // Don't draw if size == 0
  if (r.size == glm::vec2(0)) {
    return;
  }

  *arena_typed_push(batch.verticies_arena) = vertex{r.pos, r.uva};
  *arena_typed_push(batch.verticies_arena) =
      vertex{glm::vec2(r.pos.x, r.pos.y + r.size.y), glm::vec2(r.uva.x, r.uvb.y)};
  *arena_typed_push(batch.verticies_arena) =
      vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y + r.size.y), r.uvb};
  *arena_typed_push(batch.verticies_arena) =
      vertex{glm::vec2(r.pos.x + r.size.x, r.pos.y), glm::vec2{r.uvb.x, r.uva.y}};

  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 0;
  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 1;
  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 2;
  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 0;
  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 2;
  *arena_typed_push(batch.indicies_arena) = batch.data.vertex_count + 3;

  batch.data.index_count += 6;
  batch.data.vertex_count += 4;
}

void update_gpu_mesh(app_state *state, render_batch &batch) {
  state->api.renderer.update_mesh(&batch.mesh, &batch.data);
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