#include "asset_set.hh"
#include "common/file_utils.hh"
#include "common/memory_arena.hh"
#include "common/memory_scratch_arena.hh"
#include "common/result.hh"
#include "common/sdef_parser.hh"
#include "data/asset_storage.hh"
#include "data/asset_types.hh"
#include <string.h>

result<asset_type> asset_type_from_str(const char *str) {

  // TODO PHF or simmilar
  if (strcmp(str, "texture") == 0) {
    return result_ok(ASSET_TYPE_TEXTURE);
  } else if (strcmp(str, "pipeline") == 0) {
    return result_ok(ASSET_TYPE_PIPELINE);
  } else {
    return result_err<asset_type>("Could not determine asset type %s", str);
  }
}

result<texture_filter> tex_filter_string_to_filter(const char *str) {
  if (strcmp(str, "BILINEAR") == 0)
    return result_ok(TEX_FILTER_LINEAR);
  if (strcmp(str, "NEAREST") == 0)
    return result_ok(TEX_FILTER_NEAREST);

  return result_err<texture_filter>("Could not determine texture filter type %s", str);
}

result<> parse_texture(mem_arena &arena, const sdef_block &block, asset_descriptor &descriptor) {
  bool upsample_defined = false;
  bool downsample_defined = false;
  bool path_defined = false;

  for (int i = 0; i < block.property_count; i++) {
    if (strcmp(block.properties[i].name, "upsample") == 0) {
      if (block.properties[i].type != SDEF_TYPE_STRING)
        return result_err("In texture %s, the value of upsample must be a string", descriptor.name);
      if (upsample_defined)
        return result_err("In texture %s, upsample redefined as %s",
                          descriptor.name,
                          block.properties[i].string);
      upsample_defined = true;

      descriptor.texture.upsample = tex_filter_string_to_filter(block.properties[i].string);
    } else if (strcmp(block.properties[i].name, "downsample") == 0) {
      if (block.properties[i].type != SDEF_TYPE_STRING)
        return result_err("In texture %s, the value of downsample must be a string",
                          descriptor.name);
      if (downsample_defined)
        return result_err("In texture %s, downsample redefined as %s",
                          descriptor.name,
                          block.properties[i].string);
      downsample_defined = true;

      descriptor.texture.downsample = tex_filter_string_to_filter(block.properties[i].string);
    } else if (strcmp(block.properties[i].name, "file")) {
      if (block.properties[i].type != SDEF_TYPE_STRING)
        return result_err("In texture %s, the value of file must be a string", descriptor.name);
      if (path_defined)
        return result_err("In texture %s, file redefined as %s",
                          descriptor.name,
                          block.properties[i].string);

      descriptor.texture.file_path =
          arena_push_null_terminated_string(arena, block.properties[i].string);
      path_defined = true;
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
    case ASSET_TYPE_TEXTURE:
      parse_texture(arena, dom->blocks[i], s.descriptors[i]);
      break;
    case ASSET_TYPE_PIPELINE:
      break;
    default:
      // Should not happen
      return result_err<asset_set>("Type was not found");
      break;
    }
  }

  arena_scratch_free(scratch);
  return result_ok(s);
}