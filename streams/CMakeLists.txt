add_library(streams_lib STATIC)

target_sources(streams_lib PRIVATE src/Streams.cpp)

target_include_directories(streams_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(streams_lib PUBLIC seq_lib)