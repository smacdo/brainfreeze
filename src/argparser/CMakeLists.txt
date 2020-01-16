cmake_minimum_required(VERSION 3.2)
project(argparser)

set(SOURCES argparser/argparser.h argparser.cpp argparserexception.cpp)

add_library(argparser STATIC ${SOURCES})
target_include_directories(argparser PUBLIC argparser)
target_compile_features(argparser PUBLIC cxx_std_17)
set_target_properties(argparser PROPERTIES CXX_EXTENSIONS OFF)