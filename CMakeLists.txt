cmake_minimum_required(VERSION 3.20)
project(FibonacciCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


# ------------------- Основное приложение -------------------
add_executable(fib_main main.cpp)

# ------------------- GoogleTest -------------------
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ------------------- Тесты -------------------
enable_testing()

add_executable(tests tests.cpp)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(fib_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)