#pragma once

// This is the interface to the code used to load assets from disk, cache them in memory and upload
// them to the gpu. It also provides interfaces to create assets from memory.

// Assets are stored in bundles loaded in a pool it is inteded to be like that so that we can load
// assets for the main game while already being on the main menu
// +----------------------+
// |     Loading screen   |
// +----------------------+
// |     Icons and UI     |
// +----------------------+
// |        ...           |
// +----------------------+
// |    Game level data   |
// +----------------------+

#include "data/app_state.hh"
#include "data/asset_types.hh"
#include "common/win32_export.hh"

// Called by the engine
APIFUNC extern void asset_system_init(app_state *state);
// Called by the engine
APIFUNC extern void asset_system_shutdown(app_state *state);

// Used to create a bundle
APIFUNC extern uint32_t asset_bundle_create(app_state *state, const char *name);
// Used to delete a bundle
APIFUNC extern void asset_bundle_free(app_state *state, uint32_t bundle);
// Used to load the data of a bundle from the filesystem
APIFUNC extern void asset_bundle_load_file(app_state *state, uint32_t bundle, const char *filename);
// Used to save the bundle to the filesystem
APIFUNC extern void asset_bundle_save_file(app_state *state, uint32_t bundle, const char *filename);

// Used to create a texture. If texture_data is not nullptr, it will also upload the data into gpu
// memory. The user is responsible for the management of data's memor
APIFUNC extern uint32_t asset_texture_create(app_state *state, uint32_t bundle, texture_data *data);
// Used to upload the data to gpu memory. The user is responsible for the management of data's
// memory
APIFUNC extern uint32_t asset_texture_update(app_state *state,
                                     uint32_t bundle,
                                     uint32_t id,
                                     texture_data *data);
// Used to delete the texture
APIFUNC extern void asset_texture_delete(app_state *state, uint32_t bundle, uint32_t id);
// Used to get the renderer id of the texture id
APIFUNC extern renderer_texture asset_texture_get_render(app_state *state, uint32_t bundle, uint32_t id);

// Used to create a pipeline. If pipeline_data must be defined
APIFUNC extern uint32_t asset_pipeline_create(app_state *state, uint32_t bundle, pipeline_data *data);
// Used to delete the pipeline
APIFUNC extern void asset_pipeline_delete(app_state *state, uint32_t bundle, uint32_t id);
// Used to get the renderer id of the pipeline id
APIFUNC extern renderer_pipeline asset_pipeline_get_render(app_state *state, uint32_t bundle, uint32_t id);

// Used to create a mesh. If mesh_data is not nullptr, it will also upload the data into gpu
// memory. The user is responsible for the management of data's memory
APIFUNC extern uint32_t asset_mesh_create(app_state *state, uint32_t bundle, mesh_data *data);
// Used to upload the data to gpu memory. The user is responsible for the management of data's
// memory
APIFUNC extern uint32_t asset_mesh_update(app_state *state, uint32_t bundle, uint32_t id, mesh_data *data);
// Used to delete the mesh
APIFUNC extern void asset_mesh_delete(app_state *state, uint32_t bundle, uint32_t id);
// Used to get the renderer id of the mesh id
APIFUNC extern renderer_mesh asset_mesh_get_render(app_state *state, uint32_t bundle, uint32_t id);
