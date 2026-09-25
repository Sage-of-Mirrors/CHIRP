file(GLOB_RECURSE LIBCHIRP_IR_SRC
    lib/libchirp-ir/src/*.cpp
    lib/libchirp-ir/include/*.hpp
)

add_library(libchirp-ir STATIC ${LIBCHIRP_IR_SRC})
add_library(chirp::ir ALIAS libchirp-ir)

set_target_properties(libchirp-ir PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp-ir PRIVATE chirp::common)