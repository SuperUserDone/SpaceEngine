

typedef void (*loaded_proc)(void);
typedef loaded_proc (*load_proc)(const char *);
typedef bool (*renderer_init_fun)(load_proc);
typedef bool (*renderer_shutdown_fun)();
typedef void (*renderer_set_viewport_fun)(int x, int y);
typedef void (*renderer_clear_fun)(float r, float g, float b, float a);

struct renderer_api {
  renderer_init_fun init;
  renderer_shutdown_fun shutdown;
  renderer_set_viewport_fun set_viewport;
  renderer_clear_fun clear;
};

typedef renderer_api (*renderer_fetch_api_fun)();