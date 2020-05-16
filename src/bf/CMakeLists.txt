cmake_minimum_required(VERSION 3.2)
project(brainfreeze-interpreter)

# Fix for MSVC multiple warning levels. See: https://gitlab.kitware.com/cmake/cmake/issues/18317
cmake_policy(SET CMP0092 NEW)

add_library(brainfreeze-interpreter STATIC
	compiler.cpp
	helpers.cpp
	iconsole.cpp
	instruction.cpp
	interpreter.cpp
	public/bf/bf.h)
target_include_directories(brainfreeze-interpreter PUBLIC public)
target_link_libraries(brainfreeze-interpreter)
target_compile_features(brainfreeze-interpreter PUBLIC cxx_std_17)
set_target_properties(brainfreeze-interpreter PROPERTIES CXX_EXTENSIONS OFF)