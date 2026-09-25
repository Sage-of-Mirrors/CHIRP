file(GLOB_RECURSE LIBCHIRP_COM_SRC
    lib/libchirp-common/src/*.cpp
    lib/libchirp-common/include/*.hpp
)

add_library(libchirp-common STATIC ${LIBCHIRP_COM_SRC})
add_library(chirp::common ALIAS libchirp-common)

set_target_properties(libchirp-common PROPERTIES LINKER_LANGUAGE CXX)
