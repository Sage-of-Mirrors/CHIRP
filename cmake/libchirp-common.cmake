file(GLOB_RECURSE LIBCHIRP_COM_SRC
    lib/libchirp-common/*.cpp
    lib/libchirp-common/*.h
	lib/libchirp-common/*.hpp
	include/libchirp-common/*.h
	include/libchirp-common/*.hpp
)

set(LIBCHIRP_COMMON_INCLUDE_DIR include/libchirp-common)

add_library(libchirp_common STATIC ${LIBCHIRP_COM_SRC})
add_library(chirp::common ALIAS libchirp_common)

set_target_properties(libchirp_common PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(libchirp_common PUBLIC include)
