#pragma once

// This file provides the interface for the main input system of the engine.

#include "common/win32_export.hh"
#include "input/mouse_keyboard.hh"

struct app_state;

namespace input {

enum input_device_type {
  INPUT_DEVICE_MOUSE_KEYBOARD,
};

struct raw_input_state {
  input_device_type active_input;

  input_mouse_keyboard mouse_keyboard;
};

APIFUNC void reset(app_state *state);
APIFUNC void debug_ui(app_state *state);

} // namespace input