cmake_minimum_required(VERSION 4.0)
project(game)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(game ${SOURCE_FILES})
