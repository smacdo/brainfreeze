cmake_minimum_required(VERSION 3.15)
project(brainfreeze-cli)

if(WIN32)
	set(PLATFORM_CONSOLE_CPP
		platform/windows/windows_console.cpp)
else()
	set(PLATFORM_CONSOLE_CPP
		platform/unix/unix_console.cpp)
endif()

add_executable(
	brainfreeze
	cli.cpp
	platform/console.cpp
	platform/exception.cpp
	platform/posix_exception.cpp
	${PLATFORM_CONSOLE_CPP})
target_link_libraries(brainfreeze brainfreeze-interpreter CLI11 loguru)
target_compile_features(brainfreeze PUBLIC cxx_std_17)
set_target_properties(brainfreeze PROPERTIES CXX_EXTENSIONS OFF)