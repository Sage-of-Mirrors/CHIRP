file(GLOB_RECURSE LIBCHIRP_BIN_SRC
    lib/libchirp-binary/src/*.cpp
    lib/libchirp-binary/include/*.hpp
)

add_library(libchirp-binary STATIC ${LIBCHIRP_BIN_SRC})
add_library(chirp::binary ALIAS libchirp-binary)

set_target_properties(libchirp-binary PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp-binary PRIVATE chirp::common)