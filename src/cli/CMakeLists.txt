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

target_link_libraries(brainfreeze PRIVATE brainfreeze-interpreter CLI11)
target_compile_features(brainfreeze PUBLIC cxx_std_17)
set_target_properties(brainfreeze PROPERTIES CXX_EXTENSIONS OFF)

# Sanitizer support
include(CppSanitizers)
target_use_cpp_sanitizer(brainfreeze ${CPP_SANITIZER})

# Clang tidy support.
if(USE_CLANG_TIDY)
	message("enabling clang tidy via ${CLANG_TIDY_PROPERTIES}")
    set_target_properties(brainfreeze PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_PROPERTIES}")
endif() 

install(TARGETS brainfreeze BUNDLE)
