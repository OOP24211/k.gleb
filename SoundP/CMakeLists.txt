cmake_minimum_required(VERSION 3.20)
project(SoundP)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(SoundP ${SOURCE_FILES})
target_include_directories(SoundP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_custom_command(
        TARGET SoundP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/objects"
        "$<TARGET_FILE_DIR:SoundP>/objects"
)
add_custom_command(
        TARGET SoundP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:SoundP>/config"
)

