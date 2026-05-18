cmake_minimum_required(VERSION 3.20)
project(LazySequences CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Опция для сборки тестов
option(BUILD_TESTING "Build the test suite" ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Фикс рантайма для GTest и MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# ВЫКЛЮЧАЕМ ТЕСТЫ ДЛЯ СТОРОННИХ БИБЛИОТЕК (чтобы не собирать тесты самого GTest)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ВКЛЮЧАЕМ ТЕСТЫ ОБРАТНО ДЛЯ НАШЕГО ПРОЕКТА
set(BUILD_TESTING ON CACHE BOOL "" FORCE)
enable_testing()
include(GoogleTest)

if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
            CACHE STRING "" FORCE
    )
    add_compile_options(/utf-8)
endif()

add_subdirectory(core)
add_subdirectory(lazy)
add_subdirectory(streams)
add_subdirectory(fsm)
add_subdirectory(app)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()








