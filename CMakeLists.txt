cmake_minimum_required(VERSION 3.25)
project(chirp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CHIRP_BUILD_CLI "Build the main CHIRP CLI tool")
option(CHIRP_BUILD_BINARY_LIB "Build libchirp_binary")
option(CHIRP_BUILD_IR_LIB "Build libchirp_ir")

add_subdirectory(projects/libchirp_common)

if (CHIRP_BUILD_BINARY_LIB)
  add_subdirectory(projects/libchirp_binary)
endif()
if (CHIRP_BUILD_IR_LIB)
  add_subdirectory(projects/libchirp_ir)
endif()
if (CHIRP_BUILD_CLI)
  add_subdirectory(projects/chirp)
endif()
