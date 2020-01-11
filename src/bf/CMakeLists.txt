cmake_minimum_required(VERSION 3.2)
project(brainfreeze-interpreter)

set(SOURCES
	bf.cpp
	bfutils.cpp)

add_library(brainfreeze-interpreter STATIC ${SOURCES})
target_include_directories(brainfreeze-interpreter PUBLIC bf)
target_compile_features(brainfreeze-interpreter PUBLIC cxx_std_17)
set_target_properties(brainfreeze-interpreter PROPERTIES CXX_EXTENSIONS OFF)