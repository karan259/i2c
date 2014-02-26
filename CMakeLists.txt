cmake_minimum_required(VERSION 2.8)

##
## Package-wide configuration
##
project(libi2c)
set (pkg_NAME "libi2c")
set(pkg_BRIEF "RoadNarrows i2c library.")

set(pkg_VERSION_MAJ 1)
set(pkg_VERSION_MIN 2)
set(pkg_VERSION_REV 2)
set(pkg_VERSION "${pkg_VERSION_MAJ}.${pkg_VERSION_MIN}.${pkg_VERSION_REV}")

set(pkg_CREATED "2005")
execute_process(COMMAND date 
  OUTPUT_VARIABLE pkg_TIMESTAMP 
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(pkg_FULL_NAME "${pkg_NAME}-${pkg_VERSION}")
set(pkg_AUTHORS "Robin Knight, Kim Wheeler, Daniel Packard")
set(pkg_OWNERS "RoadNarrows LLC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG -DLOGMOD='\"libi2c\"'")

## configure jenkins
if(${JENKINS})
  message("Using Jenkins")
  include_directories(BEFORE ${CMAKE_INSTALL_PREFIX}/include)
  link_directories(${CMAKE_INSTALL_PREFIX}/lib/rnr)
endif(${JENKINS})

## generate package headers
configure_file (
  "${PROJECT_SOURCE_DIR}/include/version.h.in"
  "${PROJECT_BINARY_DIR}/include/version.h"
)

## add project include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include
)

## add libraries and other binaries
set(libi2c_SOURCES 
  src/libi2c/serdev.c
)

add_library(i2c-static STATIC ${libi2c_SOURCES})
set_target_properties(i2c-static 
  PROPERTIES OUTPUT_NAME i2c
  CLEAN_DIRECT_OUTPUT 1
)

add_library(i2c-shared SHARED ${libi2c_SOURCES})
set_target_properties(i2c-shared 
  PROPERTIES OUTPUT_NAME i2c
  CLEAN_DIRECT_OUTPUT 1
)

## add libraries/binaries/headers to installation
install(TARGETS i2c-static i2c-shared DESTINATION lib/rnr)
install(FILES  
    include/rnr/serdev.h
  DESTINATION 
    include/rnr
)

## configure debain package
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_NAME "libi2c-dev")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "RoadNarrows Robotics i2c library")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "librnr-dev(>=1.12.0)")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel Packard <daniel@roadnarrows.com>")
SET(CPACK_PACKAGE_VERSION ${pkg_VERSION})
SET(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/README.md) 
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
include(CPack)
