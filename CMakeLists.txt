cmake_minimum_required(VERSION 4.0)
project(game)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sfml-cmake/lib/cmake/SFML")
set(CMAKE_PREFIX_PATH ${SFML_DIR})

find_package(SFML REQUIRED COMPONENTS system window graphics)


set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(game ${SOURCE_FILES})

target_link_libraries(game sfml-graphics sfml-window sfml-system)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/obj"
        "$<TARGET_FILE_DIR:game>/obj"
)

add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/sfml"
        "$<TARGET_FILE_DIR:game>"
)