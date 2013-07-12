remake_find_library(
  tulibs-serial serial.h
  PACKAGE tulibs
)

remake_add_library(
  serial
  FORCE_LINK interface ${TULIBS_SERIAL_LIBRARY} "-Wl,-soname=libcan.so"
)
remake_add_headers()
