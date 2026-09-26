file(GLOB_RECURSE LIBCHIRP_BIN_SRC
    lib/libchirp-binary/*.cpp
    lib/libchirp-binary/*.h
	lib/libchirp-binary/*.hpp
	include/libchirp-binary/*.h
	include/libchirp-binary/*.hpp
)

set(LIBCHIRP_BINARY_INCLUDE_DIR include/libchirp-binary)

add_library(libchirp_binary STATIC ${LIBCHIRP_BIN_SRC})
add_library(chirp::binary ALIAS libchirp_binary)

set_target_properties(libchirp_binary PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp_binary PUBLIC chirp::common)
target_include_directories(libchirp_binary PUBLIC include)
