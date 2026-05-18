cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0135 NEW)
project(LazySequences CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CMAKE_MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
            CACHE STRING "" FORCE
    )
    add_compile_options(/utf-8)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Опция для сборки тестов
option(BUILD_TESTING "Build the test suite" ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Указываем, где лежит базовый ImGui (для imgui-sfml)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui CACHE PATH "" FORCE)

# ВЫКЛЮЧАЕМ ТЕСТЫ СТОРОННИХ БИБЛИОТЕК
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# ПОДКЛЮЧАЕМ ЛОКАЛЬНЫЕ ПАПКИ 
add_subdirectory(external/googletest)
add_subdirectory(external/sfml)
add_subdirectory(external/imgui_sfml) 

# ВКЛЮЧАЕМ ТЕСТЫ ОБРАТНО ДЛЯ НАШЕГО ПРОЕКТА
set(BUILD_TESTING ON CACHE BOOL "" FORCE)
enable_testing()
include(GoogleTest)

add_subdirectory(core)
add_subdirectory(lazy)
add_subdirectory(streams)
add_subdirectory(fsm)
add_subdirectory(app)


if(BUILD_TESTING)
    add_subdirectory(tests)
endif()




