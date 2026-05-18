# Создаем исполняемый файл для тестов. Сюда мы передаем наш файл с макросами TEST()
add_executable(run_tests
        test_dynamic_array.cpp
        test_linked_list.cpp
        test_array_sequence.cpp
        test_list_sequence.cpp
        test_mutability.cpp
        test_bit_sequence.cpp
        test_enumerator.cpp
        test_option_try.cpp
        test_operators.cpp
        test_builder.cpp
        test_transformations.cpp
        test_sequence.cpp
)

# Самая важная магия линковки:
# 1. Прикрепляем нашу библиотеку seq_lib, чтобы тесты "видели" классы DynamicArray, ListSequence и т.д.
# 2. Прикрепляем GTest::gtest — саму библиотеку Google Test (макросы EXPECT_EQ, EXPECT_THROW).
# 3. Прикрепляем GTest::gtest_main — это готовая реализация функции int main(),
#    которая сама найдет все тесты и запустит их.
target_link_libraries(run_tests PRIVATE seq_lib GTest::gtest GTest::gtest_main)

# Сообщаем CTest, что бинарник run_tests содержит тесты Google Test.
# Это позволит IDE (CLion, VS Code, Visual Studio) красиво отображать список тестов
# в графическом интерфейсе, с зелеными галочками при успехе.
gtest_discover_tests(run_tests)