file(GLOB_RECURSE LIBCHIRP_COM_SRC
    lib/libchirp-common/*.cpp
    lib/libchirp-common/*.h
	include/libchirp-common/*.h
)

set(LIBCHIRP_COMMON_INCLUDE_DIR include/libchirp-common)

add_library(libchirp-common STATIC ${LIBCHIRP_COM_SRC})
add_library(chirp::common ALIAS libchirp-common)

set_target_properties(libchirp-common PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(libchirp-common PUBLIC include)
