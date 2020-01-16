cmake_minimum_required(VERSION 3.15)
project(brainfreeze-cli)

set(SOURCES cli.cpp)

add_executable(brainfreeze ${SOURCES})
target_link_libraries(brainfreeze brainfreeze-interpreter argparser)
target_compile_features(brainfreeze PUBLIC cxx_std_17)
set_target_properties(brainfreeze PROPERTIES CXX_EXTENSIONS OFF)