remake_find_library(
  tulibs-serial serial.h
  PACKAGE tulibs
)

remake_include(
  ../config
  ../can
)
remake_add_library(
  serial
  LINK can ${TULIBS_SERIAL_LIBRARY}
)
remake_add_headers()
