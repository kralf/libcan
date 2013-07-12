remake_find_library(
  cpc cpc.h
  PACKAGE libcpc
)

remake_add_library(
  cpc
  LINK interface ${CPC_LIBRARY}
)
remake_add_headers()
