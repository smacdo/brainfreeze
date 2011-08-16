cmake_minimum_required(VERSION 2.6)
project(BrainFreeze)

set(GTEST_ROOT "${PROJECT_SOURCE_DIR}/extra/googletest")

add_subdirectory(extra/googletest)
add_subdirectory(src)
add_subdirectory(tests)
