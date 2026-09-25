cmake_minimum_required(VERSION 3.25)
project(chirp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CHIRP_BUILD_CLI "Build the main CHIRP CLI tool")
option(CHIRP_BUILD_BINARY_LIB "Build libchirp_binary")
option(CHIRP_BUILD_IR_LIB "Build libchirp_ir")

add_subdirectory(extern)

include(cmake/libchirp-common.cmake)

if (CHIRP_BUILD_BINARY_LIB)
  include(cmake/libchirp-binary.cmake)
endif()
if (CHIRP_BUILD_IR_LIB)
  include(cmake/libchirp-ir.cmake)
endif()
if (CHIRP_BUILD_CLI)
  include(cmake/chirp-cli.cmake)
endif()
