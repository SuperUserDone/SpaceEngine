add_library(pyro STATIC memory/arena.cc)
target_link_libraries(pyro xxHash::xxhash)