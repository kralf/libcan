remake_find_library(
  tulibs-serial serial.h
  PACKAGE tulibs
)

remake_add_library(
  serial
  LINK interface ${TULIBS_SERIAL_LIBRARY}
)
remake_add_headers()
