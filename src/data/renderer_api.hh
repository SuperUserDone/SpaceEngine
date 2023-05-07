#pragma once

#include "common/memory_arena.hh"
#include "data/asset_types.hh"

#include "common/pipeline_settings.hh"

typedef void (*loaded_proc)(void);
typedef loaded_proc (*load_proc)(const char *);

struct app_state;

typedef bool (*renderer_init_fun)(mem_arena &scratch, app_state *state, load_proc);
typedef bool (*renderer_shutdown_fun)();
typedef void (*renderer_set_viewport_fun)(int x, int y);
typedef void (*renderer_clear_fun)(float r, float g, float b, float a);

typedef renderer_texture (*renderer_create_texture_fun)(texture_data *data);
typedef void (*renderer_update_texture_fun)(renderer_texture *tex, texture_data *data);
typedef void (*renderer_delete_texture_fun)(renderer_texture tex);

typedef renderer_pipeline (*renderer_create_pipeline_fun)(pipeline_data *data);
typedef void (*renderer_delete_pipeline_fun)(renderer_pipeline tex);

typedef renderer_mesh (*renderer_create_mesh_fun)(mesh_data *data);
typedef void (*renderer_update_mesh_fun)(renderer_mesh *tex, mesh_data *data);
typedef void (*renderer_delete_mesh_fun)(renderer_mesh tex);

typedef renderer_framebuffer (*renderer_create_framebuffer_fun)(framebuffer_data *data);
typedef void (*renderer_delete_framebuffer_fun)(renderer_framebuffer tex);
typedef void (*renderer_use_framebuffer_fun)(renderer_framebuffer tex);
typedef void (*renderer_use_default_framebuffer_fun)();

typedef void (*renderer_draw_meshes_fun)(size_t count,
                                         renderer_mesh **mesh,
                                         pipeline_settings **settings,
                                         renderer_pipeline *pipeline);
typedef void (*renderer_imgui_begin_fun)();
typedef void (*renderer_imgui_end_fun)();

typedef void (*renderer_set_blending_fun)(blending_func f);

struct renderer_api {
  renderer_init_fun init;
  renderer_shutdown_fun shutdown;
  renderer_set_viewport_fun set_viewport;
  renderer_clear_fun clear;

  renderer_create_texture_fun create_texture;
  renderer_update_texture_fun update_texture;
  renderer_delete_texture_fun delete_texture;

  renderer_create_pipeline_fun create_pipeline;
  renderer_delete_pipeline_fun delete_pipeline;

  renderer_create_mesh_fun create_mesh;
  renderer_update_mesh_fun update_mesh;
  renderer_delete_mesh_fun delete_mesh;

  renderer_create_framebuffer_fun create_framebuffer;
  renderer_delete_framebuffer_fun delete_framebuffer;
  renderer_use_framebuffer_fun use_framebuffer;
  renderer_use_default_framebuffer_fun use_default_framebuffer;

  renderer_draw_meshes_fun draw_meshes;
  renderer_imgui_begin_fun imgui_begin;
  renderer_imgui_begin_fun imgui_end;
  renderer_set_blending_fun set_blending;

};
