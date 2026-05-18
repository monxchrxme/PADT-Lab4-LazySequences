macro(add_module_test target_name source_files linked_libs)
    add_executable(${target_name} ${source_files})
    target_link_libraries(${target_name} PRIVATE ${linked_libs} GTest::gtest_main)
    gtest_discover_tests(${target_name})
endmacro()

# ТЕСТЫ ЛЕНИВОСТИ 
add_module_test(tests_lazy "cardinal_tests.cpp;context_tests.cpp;sequence_tests.cpp" lazy_lib)

# ТЕСТЫ ПОТОКОВ
add_module_test(tests_streams "streams_tests.cpp" "streams_lib;lazy_lib")

# ТЕСТЫ АВТОМАТА
add_module_test(tests_fsm "fsm_tests.cpp" fsm_lib)