add_library(lazy_lib STATIC)

target_sources(lazy_lib PRIVATE
    src/LazyContext.cpp
)

target_include_directories(lazy_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lazy_lib PUBLIC seq_lib)