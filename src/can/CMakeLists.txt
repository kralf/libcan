remake_find_library(
  tulibs-config config.h
  PACKAGE tulibs
)

remake_add_library(
  interface
  LINK ${TULIBS_CONFIG_LIBRARY}
)
remake_add_headers()
