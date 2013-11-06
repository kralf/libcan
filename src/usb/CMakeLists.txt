remake_find_package(tulibs CONFIG)

remake_add_library(
  can-usb PREFIX OFF
  *.c ../can/*.c
  LINK ${TULIBS_LIBRARIES} "-Wl,-soname=libcan.so"
)
remake_add_headers(INSTALL usb)
