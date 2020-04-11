cmake_minimum_required(VERSION 3.2)
project(
	loguru
	VERSION "2.1.0"
	HOMEPAGE_URL "https://github.com/emilk/loguru"
	DESCRIPTION "A lightweight and flexible C++ logging library")

add_library(loguru STATIC src/loguru.cpp)
target_include_directories(loguru PUBLIC include)
target_compile_features(loguru PUBLIC cxx_std_17)
set_target_properties(loguru PROPERTIES CXX_EXTENSIONS OFF)