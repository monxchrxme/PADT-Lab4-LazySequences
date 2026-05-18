set(LAZY_TEST_FILES 
    cardinal_tests.cpp 
    context_tests.cpp 
    sequence_tests.cpp
)
add_executable(tests_lazy ${LAZY_TEST_FILES})
target_link_libraries(tests_lazy PRIVATE lazy_lib GTest::gtest_main)
gtest_discover_tests(tests_lazy)


# ТЕСТЫ ПОТОКОВ 
add_executable(tests_streams streams_tests.cpp)
# Подключаем и потоки, и ленивость
target_link_libraries(tests_streams PRIVATE streams_lib lazy_lib GTest::gtest_main)
gtest_discover_tests(tests_streams)


# ТЕСТЫ АВТОМАТА 
add_executable(tests_fsm fsm_tests.cpp)
target_link_libraries(tests_fsm PRIVATE fsm_lib GTest::gtest_main)
gtest_discover_tests(tests_fsm)