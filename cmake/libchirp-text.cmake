file(GLOB_RECURSE LIBCHIRP_TEXT_SRC
    lib/libchirp-text/*.cpp
    lib/libchirp-text/*.hpp
	include/libchirp-text/*.hpp
)

set(LIBCHIRP_TEXT_INCLUDE_DIR include/libchirp-text)

add_library(libchirp_text STATIC ${LIBCHIRP_TEXT_SRC})
add_library(chirp::text ALIAS libchirp_text)

set_target_properties(libchirp_text PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp_text PUBLIC chirp::common)
target_include_directories(libchirp_text PUBLIC include)
