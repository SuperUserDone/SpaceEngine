#include "data/app_state.hh"

void init(mem_arena &permanent_arena, app_state *state) {
}
void update(mem_arena &permanent_arena, mem_arena &frame_arena, app_state *state) {
}
void shutdown(mem_arena &permanent_arena, app_state *state) {
}

extern "C" {
APIFUNC void fetch_api(app_state *state) {
  state->api.game.init = init;
  state->api.game.update = update;
  state->api.game.shutdown = shutdown;
}
}