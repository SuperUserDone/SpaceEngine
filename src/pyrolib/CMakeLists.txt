add_library(pyro STATIC memory/arena.cc utils/timer.cc)
target_link_libraries(pyro xxHash::xxhash)