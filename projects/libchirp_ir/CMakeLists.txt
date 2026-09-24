file(GLOB_RECURSE LIBCHIRPIR_SRC
    src/*.cpp
    include/*.hpp
)

add_library(libchirp_ir STATIC
    ${LIBCHIRPIR_SRC}
)

add_library(chirp::ir ALIAS libchirp_ir)

set_target_properties(libchirp_ir PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp_ir PRIVATE chirp::common)