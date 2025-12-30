cmake_minimum_required(VERSION 3.14)
project(lab0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(lab0 ${SOURCE_FILES})


target_include_directories(lab0 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)




