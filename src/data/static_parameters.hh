#pragma once

#define SPACE_BUDGET_GAME_RENDER 4000         // 4ms
#define SPACE_BUDGET_INTER_CPUGPU_MEMCOPY 100 // 0.1ms
#define SPACE_BUDGET_GAME_UPDATE 8000         // 8ms
#define SPACE_BUDGET_UI_UPDATE_RENDER 1000    // 1ms
// This can be moved into separate thread(s) but for now we will keep it here
#define SPACE_BUDGET_MESH_UPDATE 2000 // 2ms