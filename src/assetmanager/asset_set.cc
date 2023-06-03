// This file is a mess. Many of the things in here can be optimized, cleaned or otherwise

#include "assetmanager/asset_set.hh"
#include "common/file_utils.hh"
#include "common/result.hh"
#include "common/sdef_parser.hh"
#include "data/asset_storage.hh"
#include "memory/memory_scratch_arena.hh"

#include <string>
#include <unordered_map>

enum texture_properties {
  TP_UPSAMPLE,
  TP_DOWNSAMPLE,
  TP_PATH,
};

enum pipeline_properties {
  PP_FRAG,
  PP_VERT,
  PP_UNIFORMS,
};

enum font_properties {
  FP_PATH,
};

// The lookup tables for the enums. We use std::unordered_map here as our hashmap implementation is
// not optimized for this usecase an cannot be assigned to directly
static std::unordered_map<std::string, asset_type> asset_types = {{"texture", ASSET_TYPE_TEXTURE},
                                                                  {"pipeline", ASSET_TYPE_PIPELINE},
                                                                  {"font", ASSET_TYPE_FONT}};

static std::unordered_map<std::string, texture_filter> tex_filters = {
    {"BILINEAR", TEX_FILTER_LINEAR},
    {"NEAREST", TEX_FILTER_NEAREST}};

static std::unordered_map<std::string, texture_properties> tex_props = {
    {"upsample", TP_UPSAMPLE},
    {"downsample", TP_DOWNSAMPLE},
    {"path", TP_PATH}};

static std::unordered_map<std::string, font_properties> font_props = {{"path", FP_PATH}};

static std::unordered_map<std::string, pipeline_properties> pipeline_props = {
    {"frag", PP_FRAG},
    {"vert", PP_VERT},
    {"uniforms", PP_UNIFORMS},
};

#define map_lookup(map, val)                                                                       \
  auto it = map.find(val);                                                                         \
  if (it != map.end())                                                                             \
    return result_ok(it->second);

result<asset_type> asset_type_from_str(const char *str) {
  map_lookup(asset_types, str);
  return result_err<asset_type>("Could not determine asset type %s", str);
}

result<texture_filter> tex_filter_from_str(const char *str) {
  map_lookup(tex_filters, str);
  return result_err<texture_filter>("Could not determine texture filter type %s", str);
}

result<texture_properties> tex_prop_from_str(const char *str) {
  map_lookup(tex_props, str);
  return result_err<texture_properties>("Unknown texture property %s", str);
}

result<pipeline_properties> pipeline_prop_from_str(const char *str) {
  map_lookup(pipeline_props, str);
  return result_err<pipeline_properties>("Unknown pipeline property %s", str);
}

result<font_properties> font_prop_from_str(const char *str) {
  map_lookup(font_props, str);
  return result_err<font_properties>("Unknown pipeline property %s", str);
}

const char *sdef_type_to_string(sdef_type type) {
  switch (type) {
  case SDEF_TYPE_INTEGER:
    return "integer";
  case SDEF_TYPE_STRING:
    return "string";
  case SDEF_TYPE_STRING_ARRAY:
    return "array";
    break;
  }
}

result<> typecheck(const char *name,
                   const char *prop_name,
                   sdef_type type,
                   bool already_defined,
                   const sdef_property &prop) {
  if (prop.type != type)
    return result_err("In %s, the value of %s must be a %s", name, type, sdef_type_to_string(type));
  if (already_defined)
    return result_err("In %s, %s redefined as %s", name, type, prop.string);

  return result_ok(true);
}

result<> parse_texture(mem_arena &arena, const sdef_block &block, asset_descriptor &descriptor) {
  bool upsample_defined = false;
  bool downsample_defined = false;
  bool path_defined = false;

  for (int i = 0; i < block.property_count; i++) {

    result_forward_err(prop_type, tex_prop_from_str(block.properties[i].name));
    switch (prop_type) {

    case TP_UPSAMPLE: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "upsample",
                                   SDEF_TYPE_STRING,
                                   upsample_defined,
                                   block.properties[i]));
      upsample_defined = true;

      result_forward_err(val, tex_filter_from_str(block.properties[i].string));
      descriptor.texture.upsample = val;
      break;
    }

    case TP_DOWNSAMPLE: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "downsample",
                                   SDEF_TYPE_STRING,
                                   downsample_defined,
                                   block.properties[i]));
      downsample_defined = true;

      result_forward_err(val, tex_filter_from_str(block.properties[i].string));
      descriptor.texture.downsample = val;
      break;
    }
    case TP_PATH: {
      result_forward_err(
          _,
          typecheck(descriptor.name, "path", SDEF_TYPE_STRING, path_defined, block.properties[i]));

      path_defined = true;
      descriptor.texture.file_path =
          arena_push_null_terminated_string(arena, block.properties[i].string);
      break;
    }
    default:
      return result_err("In texture %s, unknown property %s",
                        descriptor.name,
                        block.properties[i].name);
    }
  }

  if (!upsample_defined)
    return result_err("In texture %s upsample was not defined", descriptor.name);
  if (!downsample_defined)
    return result_err("In texture %s downsample was not defined", descriptor.name);
  if (!path_defined)
    return result_err("In texture %s a path was not defined", descriptor.name);

  return result_ok(true);
}

result<> parse_pipeline(mem_arena &arena, const sdef_block &block, asset_descriptor &descriptor) {
  bool frag_defined = false;
  bool vert_defined = false;
  bool uniform_defined = false;

  for (int i = 0; i < block.property_count; i++) {
    result_forward_err(prop_type, pipeline_prop_from_str(block.properties[i].name));
    switch (prop_type) {
    case PP_VERT: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "vertex",
                                   SDEF_TYPE_STRING,
                                   vert_defined,
                                   block.properties[i]));

      descriptor.pipeline.vertex =
          arena_push_null_terminated_string(arena, block.properties[i].string);
      vert_defined = true;
      break;
    }
    case PP_FRAG: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "fragment",
                                   SDEF_TYPE_STRING,
                                   frag_defined,
                                   block.properties[i]));

      descriptor.pipeline.fragment =
          arena_push_null_terminated_string(arena, block.properties[i].string);
      frag_defined = true;
      break;
    }
    case PP_UNIFORMS: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "uniforms",
                                   SDEF_TYPE_STRING_ARRAY,
                                   uniform_defined,
                                   block.properties[i]));

      char *uniforms_info =
          arena_push_string(arena, block.properties[i].string_array, block.properties[i].total_len);

      size_t j = 0;

      descriptor.pipeline.uniforms = (char **)arena_push(arena, 0);
      descriptor.pipeline.uniforms_count = 0;

      while (j < block.properties[i].total_len) {
        size_t len = strlen(&uniforms_info[j]);
        arena_push(arena, sizeof(char *));
        descriptor.pipeline.uniforms[descriptor.pipeline.uniforms_count++] = &uniforms_info[j];
        j += len + 1;
      }

      uniform_defined = true;
      break;
    }
    default:
      return result_err("In pipeline %s, unknown property %s",
                        descriptor.name,
                        block.properties[i].name);
    }
  }

  if (!frag_defined)
    return result_err("In pipeline %s fragment shader was not defined", descriptor.name);
  if (!vert_defined)
    return result_err("In pipeline %s vertex shader was not defined", descriptor.name);
  if (!uniform_defined)
    return result_err("In pipeline %s no uniforms were defined", descriptor.name);

  return result_ok(true);
}

result<> parse_font(mem_arena &arena, const sdef_block &block, asset_descriptor &descriptor) {
  bool path_defined = false;

  for (int i = 0; i < block.property_count; i++) {
    result_forward_err(prop_type, font_prop_from_str(block.properties[i].name));
    switch (prop_type) {
    case FP_PATH: {
      result_forward_err(_,
                         typecheck(descriptor.name,
                                   "fragment",
                                   SDEF_TYPE_STRING,
                                   path_defined,
                                   block.properties[i]));

      descriptor.font.path = arena_push_null_terminated_string(arena, block.properties[i].string);
      path_defined = true;
      break;
    }
    default:
      return result_err("In font %s, unknown property %s",
                        descriptor.name,
                        block.properties[i].name);
    }
  }

  if (!path_defined)
    return result_err("In font %s no path was defined", descriptor.name);

  return result_ok(true);
}

result<asset_set> asset_set_load_from_file(mem_arena &arena, const char *filename) {
  mem_scratch_arena scratch = arena_scratch_get();
  const char *file_content = load_file(scratch, filename);
  result_forward_err(dom, sdef_parse(scratch, file_content, strlen(file_content)));

  asset_set s;
  // Allocate descriptors in the set
  s.descriptors = arena_push_array(arena, asset_descriptor, dom->block_count);
  s.count = dom->block_count;

  for (int i = 0; i < s.count; i++) {
    result_forward_err(type, asset_type_from_str(dom->blocks[i].type));
    s.descriptors[i].type = type;
    s.descriptors[i].name = arena_push_null_terminated_string(arena, dom->blocks[i].name);

    switch (type) {
    case ASSET_TYPE_TEXTURE: {
      result_forward_err(_, parse_texture(arena, dom->blocks[i], s.descriptors[i]));
      break;
    }
    case ASSET_TYPE_PIPELINE: {
      result_forward_err(_, parse_pipeline(arena, dom->blocks[i], s.descriptors[i]));
      break;
    }
    case ASSET_TYPE_FONT: {
      result_forward_err(_, parse_font(arena, dom->blocks[i], s.descriptors[i]));
      break;
    }
    default:
      // Should not happen
      return result_err<asset_set>("Type was not found");
      break;
    }
  }

  arena_scratch_free(scratch);
  return result_ok(s);
}
