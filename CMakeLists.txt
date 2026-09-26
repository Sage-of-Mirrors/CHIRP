cmake_minimum_required(VERSION 3.25)
project(chirp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CHIRP_BUILD_CLI "Build chirp-cli" ON)
option(CHIRP_BUILD_BINARY_LIB "Build libchirp-binary" ON)
option(CHIRP_BUILD_TEXT_LIB "Build libchirp-text" ON)

add_subdirectory(extern)

include(cmake/libchirp-common.cmake)

if (CHIRP_BUILD_BINARY_LIB)
  include(cmake/libchirp-binary.cmake)
endif()
if (CHIRP_BUILD_TEXT_LIB)
  include(cmake/libchirp-text.cmake)
endif()
if (CHIRP_BUILD_CLI)
  include(cmake/chirp-cli.cmake)
endif()
