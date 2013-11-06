remake_find_package(tulibs CONFIG)

remake_add_library(
  can-serial PREFIX OFF
  *.c ../can/*.c
  LINK ${TULIBS_LIBRARIES} "-Wl,-soname=libcan.so"
)
remake_add_headers(INSTALL serial)
