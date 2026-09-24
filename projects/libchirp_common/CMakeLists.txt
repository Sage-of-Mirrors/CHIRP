file(GLOB_RECURSE LIBCHIRPCOM_SRC
    src/*.cpp
    include/*.hpp
)

add_library(libchirp_common STATIC
    ${LIBCHIRPCOM_SRC}
)

add_library(chirp::common ALIAS libchirp_common)

set_target_properties(libchirp_common PROPERTIES LINKER_LANGUAGE CXX)
