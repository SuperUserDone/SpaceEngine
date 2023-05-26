#pragma once

// This is the interface to the code used to load assets and upload them to the gpu. It also
// provides interfaces to create assets from memory.

#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "data/asset_types.hh"

// Called by the engine
APIFUNC extern void asset_system_init(app_state *state);
// Called by the engine
APIFUNC extern void asset_system_shutdown(app_state *state);

// Used to create a texture. If texture_data is not nullptr, it will also upload the data into gpu
// memory. The user is responsible for the management of data's memory
APIFUNC extern void asset_texture_create(app_state *state, const char *id, texture_data *data);
// Used to upload the data to gpu memory. The user is responsible for the management of data's
// memory
APIFUNC extern void asset_texture_update(app_state *state, const char *id, texture_data *data);
// Used to delete the texture
APIFUNC extern void asset_texture_delete(app_state *state, const char *id);
// Used to get the renderer id of the texture id
APIFUNC extern renderer_texture asset_texture_get_render(app_state *state, const char *id);

// Used to create a pipeline. Pipeline_data must be defined
APIFUNC extern void asset_pipeline_create(app_state *state, const char *id, pipeline_data *data);
// Used to delete the pipeline
APIFUNC extern void asset_pipeline_delete(app_state *state, const char *id);
// Used to get the renderer id of the pipeline id
APIFUNC extern renderer_pipeline asset_pipeline_get_render(app_state *state, const char *id);

// Used to create a framebuffer. Data must be defined
APIFUNC extern void asset_framebuffer_create(app_state *state,
                                             const char *id,
                                             framebuffer_data *data);
// Used to delete the framebuffer
APIFUNC extern void asset_framebuffer_delete(app_state *state, const char *id);
// Used to fetch a framebuffer
APIFUNC extern renderer_framebuffer asset_framebuffer_get_render(app_state *state, const char *id);

// Used to create a mesh. It will also upload the data into gpu memory. The user is responsible for
// the management of data's memory
APIFUNC extern void asset_mesh_create(app_state *state, const char *id, mesh_data *data);
// Used to upload the data to gpu memory. The user is responsible for the management of data's
// memory
APIFUNC extern void asset_mesh_update(app_state *state, const char *id, mesh_data *data);
// Used to delete the mesh
APIFUNC extern void asset_mesh_delete(app_state *state, const char *id);
// Used to get the renderer id of the mesh id
APIFUNC extern renderer_mesh asset_mesh_get_render(app_state *state, const char *id);
