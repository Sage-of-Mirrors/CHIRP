file(GLOB_RECURSE CHIRP_SRC
    src/*.cpp
    include/*.hpp
)

add_executable(chirp
    ${CHIRP_SRC}
)

set_target_properties(chirp PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(chirp PRIVATE chirp::common chirp::binary chirp::ir)