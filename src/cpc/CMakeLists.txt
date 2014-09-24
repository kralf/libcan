remake_find_package(libcpc CONFIG)
remake_find_library(m math.h PACKAGE libm)

remake_add_library(
  can-cpc PREFIX OFF
  *.c ../can/*.c
  LINK ${TULIBS_LIBRARIES} ${LIBCPC_LIBRARIES} ${M_LIBRARY}
    "-Wl,-soname=libcan.so"
)
remake_add_headers(INSTALL cpc)
