remake_find_library(
  cpc cpc.h
  PACKAGE libcpc
)

remake_add_library(
  cpc
  FORCE_LINK interface ${CPC_LIBRARY}
)
remake_add_headers()
