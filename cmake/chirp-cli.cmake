file(GLOB_RECURSE CHIRP_CLI_SRC
    src/*.cpp
    include/*.hpp
    include/*.h
)

add_executable(chirp-cli ${CHIRP_CLI_SRC})

set_target_properties(chirp-cli PROPERTIES OUTPUT_NAME "chirp")
target_link_libraries(chirp-cli PUBLIC chirp::common chirp::binary chirp::text)
target_include_directories(chirp-cli PUBLIC include)
