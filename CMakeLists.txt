cmake_minimum_required(VERSION 3.15)
project(BrainFreeze)

add_subdirectory(thirdparty/catch2)
add_subdirectory(src/bf)
add_subdirectory(src/cli)
add_subdirectory(src/tests)
