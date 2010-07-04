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
  tulibs
)
remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  libcan tulibs-dev
)
remake_pack_deb(
  ARCH all
  COMPONENT doc
  DESCRIPTION "documentation"
)
