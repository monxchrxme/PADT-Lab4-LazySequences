add_library(fsm_lib STATIC)

target_sources(fsm_lib PRIVATE src/StateMachine.cpp)

target_include_directories(fsm_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(fsm_lib PUBLIC lazy_lib streams_lib)