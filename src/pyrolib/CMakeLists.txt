include_directories("extern" "pyrolib")
add_library(pyro SHARED memory/arena.cc utils/timer.cc memory/scratch_arena.cc log/log.cc)

target_link_libraries(pyro PUBLIC glm::glm ${ADD_LINKS})
target_compile_definitions(pyro PRIVATE PYRO_EXPORT_WIN)
