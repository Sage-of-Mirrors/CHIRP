file(GLOB_RECURSE LIBCHIRP_BIN_SRC
    lib/libchirp-binary/*.cpp
    lib/libchirp-binary/*.h
	include/libchirp-binary/*.h
)

set(LIBCHIRP_BINARY_INCLUDE_DIR include/libchirp-binary)

add_library(libchirp-binary STATIC ${LIBCHIRP_BIN_SRC})
add_library(chirp::binary ALIAS libchirp-binary)

set_target_properties(libchirp-binary PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp-binary PUBLIC chirp::common)
target_include_directories(libchirp-binary PUBLIC include)
