cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  libcan
  VERSION 0.1
  RELEASE alpha
  SUMMARY "A simple, unified CAN communication library."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://aslforge.ethz.ch/projects/libcan"
  LICENSE "GNU Lesser General Public License (LGPL)"
  PREFIX can-
  INSTALL /usr
)

remake_doc(
  html
)

remake_pack_deb(
  DEPENDS tulibs libcpc
)
remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  DEPENDS libcan tulibs-dev libcpc-dev
)
remake_pack_deb(
  ARCH all
  COMPONENT doc
  DESCRIPTION "documentation"
)

remake_distribute_deb(
  DISTRIBUTION lucid
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS tulibs-dev libcpc-dev remake doxygen
  PASS CMAKE_BUILD_TYPE LIBCAN_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS tulibs-dev libcpc-dev remake doxygen
  PASS CMAKE_BUILD_TYPE LIBCAN_GIT_REVISION
)
