cmake_minimum_required(VERSION 3.16)
project(attiny3226_talking_thermometer C ASM)
set_source_files_properties(foo.s PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-attributes -fno-strict-aliasing")
set(CMAKE_C_FLAGS_RELEASE "-Os")

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/config.h"
  )

set(PROJECT_INCLUDES ${CMAKE_PREFIX_PATH}/include)
include_directories("${PROJECT_INCLUDES}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src)
