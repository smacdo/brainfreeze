cmake_minimum_required(VERSION 3.15)
project(BrainFreeze)

# Fix for MSVC multiple warning levels. See: https://gitlab.kitware.com/cmake/cmake/issues/18317
#cmake_policy(SET CMP0092 NEW)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(thirdparty/catch2)
add_subdirectory(src/argparser)
add_subdirectory(src/bf)
add_subdirectory(src/cli)
add_subdirectory(src/tests)
