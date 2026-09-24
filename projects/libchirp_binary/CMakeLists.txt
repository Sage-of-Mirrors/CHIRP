file(GLOB_RECURSE LIBCHIRPBIN_SRC
    src/*.cpp
    include/*.hpp
)

add_library(libchirp_binary STATIC
    ${LIBCHIRPBIN_SRC}
)

add_library(chirp::binary ALIAS libchirp_binary)

set_target_properties(libchirp_binary PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp_binary PRIVATE chirp::common)