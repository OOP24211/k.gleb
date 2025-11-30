cmake_minimum_required(VERSION 3.20)
project(FibonacciCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


# ------------------- Основное приложение -------------------

file(GLOB ALL_SOURCE_FILES
    "src/*.cpp"
    "tests/tests.cpp"
)

add_executable(lab1 src/main.cpp)

target_include_directories(lab1 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

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

add_executable(tests tests/tests.cpp)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(lab1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tests 
    GTest::gtest_main 
    main_app  
)

include(GoogleTest)
gtest_discover_tests(tests)