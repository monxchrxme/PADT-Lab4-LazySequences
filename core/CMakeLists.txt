add_subdirectory(src)

target_include_directories(seq_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/memory>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/sequences>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/types>
)

option(BUILD_CORE_TESTS "Build tests for Lab 2 (core module)" OFF)

if(BUILD_TESTING AND BUILD_CORE_TESTS)
    add_subdirectory(tests)
endif()