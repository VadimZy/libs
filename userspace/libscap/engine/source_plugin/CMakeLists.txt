include_directories(${LIBSCAP_INCLUDE_DIRS} ../noop)
add_library(scap_engine_source_plugin source_plugin.c)
target_link_libraries(scap_engine_source_plugin scap_engine_noop)
