remake_find_package(tulibs CONFIG)

remake_pkg_config_generate(EXTRA_LIBS -lcan REQUIRES tulibs)

remake_include(*)
remake_add_directories()
