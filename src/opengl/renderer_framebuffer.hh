#pragma once

#include "data/asset_types.hh"
#include "glad/gl.h"

static inline renderer_framebuffer create_framebuffer(framebuffer_data *data) {
  renderer_framebuffer fb;

  glGenFramebuffers(1, &fb.index);
  glBindFramebuffer(GL_FRAMEBUFFER, fb.index);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         data->color_attachment.index,
                         0);

  return fb;
}

static inline void delete_framebuffer(renderer_framebuffer fb) {
  glDeleteFramebuffers(1, &fb.index);
}

static inline void use_framebuffer(renderer_framebuffer fb) {
  glBindFramebuffer(GL_FRAMEBUFFER, fb.index);
}

static inline void use_default_framebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
