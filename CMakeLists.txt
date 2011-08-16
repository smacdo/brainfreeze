cmake_minimum_required(VERSION 2.6)
project(BrainFreeze)

if(MSVC)
    # Enable security checks and set strict warnings.
    # Disable l4 warnings: not using MS secure functions, dead functions and
    #                      non-existant pdb files
    set(CMAKE_CXX_FLAGS "/Gs /W4 /wd4996 /wd4505 /wd4099")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # Enable lots of extra warnings and make them errors if they happen
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -msse2")
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fomit-frame-pointer")
    message("BUILD IS " ${CMAKE_BUILD_TYPE})
endif()

# Grab Google test framework
set(GTEST_ROOT "${PROJECT_SOURCE_DIR}/extra/googletest")

###
### Build everything
###
add_subdirectory(extra/googletest)
add_subdirectory(src)
add_subdirectory(tests)
