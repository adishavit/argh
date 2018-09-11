#argh-config.cmake - package configuration file for argh library
#
# exports:
#
# sets:
#   argh_INCLUDE_DIR - the directory containing argh.h



if(NOT TARGET argh)
  include("${CMAKE_CURRENT_LIST_DIR}/arghTargets.cmake")
  get_target_property(argh_INCLUDE_DIR argh INTERFACE_INCLUDE_DIRECTORIES)
endif()
