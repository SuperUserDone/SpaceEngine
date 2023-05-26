#pragma once

// This file provides the interface for the main input system of the engine.

#include "common/result.hh"
#include "common/win32_export.hh"
#include "data/app_state.hh"
#include "input/mouse_keyboard.hh"

enum input_device_type {
  INPUT_DEVICE_MOUSE_KEYBOARD,
};

struct input_device {
  input_device_type type;

  union {
    input_mouse_keyboard pc;
  };
};

struct input_state {
  input_device_type active_input;
};

// Called by the engine
APIFUNC void input_init(app_state *state);

// Called by the engine
APIFUNC void input_reset(app_state *state);

// Used to draw input debug UI
APIFUNC void input_draw_input_debug_ui(app_state *state);
