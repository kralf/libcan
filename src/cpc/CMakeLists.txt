remake_find_library(
  cpc cpc.h
  PACKAGE libcpc
)

remake_include(
  ../config
  ../can
)
remake_add_library(
  cpc
  LINK can ${CPC_LIBRARY}
)
remake_add_headers()
