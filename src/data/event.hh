#pragma once

enum event_type {
  EVENT_TYPE_NONE,
  EVENT_TYPE_QUIT,
  EVENT_TYPE_RESIZE
};

struct event {
  event_type type;

  union {
    struct {
      int newx;
      int newy;
    } resize;
  } data;
};
