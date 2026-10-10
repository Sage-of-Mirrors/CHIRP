cmake_minimum_required(VERSION 3.25)
project(chirp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CHIRP_BUILD_CLI "Build chirp-cli" ON)
option(CHIRP_BUILD_TEXT_LIB "Build libchirp-text" ON)

add_subdirectory(extern)

include(cmake/libchirp.cmake)

if (CHIRP_BUILD_TEXT_LIB)
  include(cmake/libchirp-text.cmake)
endif()
if (CHIRP_BUILD_CLI)
  include(cmake/chirp-cli.cmake)
endif()
