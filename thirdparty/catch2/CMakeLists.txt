cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(
	catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG "v2.11.0"
	GIT_PROGRESS TRUE
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(catch2)