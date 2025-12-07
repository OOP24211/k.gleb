cmake_minimum_required(VERSION 4.0)
project(game)

set (SFML_DIR "ПУТЬ К ПАПКЕ SFML")
set(CMAKE_PREFIX_PATH ${SFML_DIR})
find_package(SFML REQUIRED COMPONENTS system window graphics)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)


add_executable(game ${SOURCE_FILES})
target_link_libraries(game sfml-system sfml-window sfml-graphics)