file(GLOB_RECURSE LIBCHIRP_SRC
    lib/libchirp/*.cpp
    lib/libchirp/*.h
	lib/libchirp/*.hpp
	include/libchirp/*.h
	include/libchirp/*.hpp
)

set(LIBCHIRP_BINARY_INCLUDE_DIR include/libchirp)

add_library(libchirp STATIC ${LIBCHIRP_SRC})
add_library(chirp ALIAS libchirp)

set_target_properties(libchirp PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(libchirp PUBLIC athena-core)
target_include_directories(libchirp PUBLIC include extern/athena/include)
