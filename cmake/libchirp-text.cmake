file(GLOB_RECURSE LIBCHIRP_TEXT_SRC
    lib/libchirp-text/*.cpp
    lib/libchirp-text/*.h
	include/libchirp-text/*.h
)

set(LIBCHIRP_TEXT_INCLUDE_DIR include/libchirp-text)

add_library(libchirp-text STATIC ${LIBCHIRP_TEXT_SRC})
add_library(chirp::text ALIAS libchirp-text)

set_target_properties(libchirp-text PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp-text PUBLIC chirp::common)
target_include_directories(libchirp-text PUBLIC include)
