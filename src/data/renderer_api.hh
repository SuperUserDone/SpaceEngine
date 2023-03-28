#pragma once

#include "common/memory_arena.hh"
#include "data/mesh.hh"

typedef void (*loaded_proc)(void);
typedef loaded_proc (*load_proc)(const char *);

enum mesh_render_material_type {
  OPAQUE_COLOR = 0,
  COUNT
};

struct mesh_render_material {
  mesh_render_material_type type;

  union {
    struct {
      float r;
      float g;
      float b;
    } opaque_color;
  };
};

struct app_state;

typedef bool (*renderer_init_fun)(mem_arena &scratch, app_state *state, load_proc);
typedef bool (*renderer_shutdown_fun)();
typedef void (*renderer_set_viewport_fun)(int x, int y);
typedef void (*renderer_clear_fun)(float r, float g, float b, float a);
typedef void (*renderer_render_mesh_fun)(app_state *state,
                                         mesh &mesh_data,
                                         mesh_render_material &mat);

struct renderer_api {
  renderer_init_fun init;
  renderer_shutdown_fun shutdown;
  renderer_set_viewport_fun set_viewport;
  renderer_clear_fun clear;
  renderer_render_mesh_fun render_mesh;
};
