#pragma once

#include <stdint.h>

namespace input {
namespace mouse {
enum button : size_t {
  MIDDLE = 0,
  LEFT,
  RIGHT,
  EXTRA_1,
  EXTRA_2,
  EXTRA_3,
  EXTRA_4,
  EXTRA_5,
  COUNT
};

static constexpr const char *mouse_button_names[] = {
    "mouse_button.middle",
    "mouse_button.left",
    "mouse_button.right",
    "mouse_button.extra_1",
    "mouse_button.extra_2",
    "mouse_button.extra_3",
    "mouse_button.extra_4",
    "mouse_button.extra_5",
};

static_assert(sizeof(mouse_button_names) / sizeof(mouse_button_names[0]) == button::COUNT,
              "The mouse name array is not Equal to number of mouse butons");

} // namespace mouse
} // namespace input