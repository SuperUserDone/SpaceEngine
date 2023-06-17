include_directories("extern")
add_library(pyro SHARED memory/arena.cc utils/timer.cc memory/scratch_arena.cc log/log.cc)


target_compile_definitions(pyro PRIVATE PYRO_EXPORT_WIN)