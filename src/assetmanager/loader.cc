#include "loader.hh"
#include "common/memory_arena.hh"

load_result *asset_loader_load(mem_arena &arena, app_state *state, asset_set &set) {
  load_result *res = arena_push_struct(arena, load_result);
  return res;
}

void asset_loader_upload_to_vram(app_state *state, load_result *result){
}
