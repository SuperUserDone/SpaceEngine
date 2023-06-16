#pragma once

#include "input/mouse_button.hh"
#include "keys.hh"
#include "pyrolib/container/arena_vector.hh"
#include "pyrolib/container/array.hh"
#include <glm/glm.hpp>
#include <stdint.h>

namespace input {
struct input_mouse_keyboard {
  //bool lock_cursor;
  //bool hide_cursor;

  glm::vec2 mouse_pos;
  int mouse_wheel_vel;

  uint8_t keys[keyboard::key::COUNT];
  uint8_t mouse[mouse::button::COUNT];
};
} // namespace input
