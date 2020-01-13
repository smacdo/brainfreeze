cmake_minimum_required(VERSION 3.2)
project(brainfreeze-tests)

set(SOURCES
	testrunner.cpp
	testhelpers.cpp
	testhelpers.h
)

set(TEST_FILES
    argparser_tests.cpp
    compiler_tests.cpp
	instruction_tests.cpp
	interpreter_tests.cpp
	jumpsearch_tests.cpp
	smoke_tests.cpp
)

add_executable(tests ${SOURCES} ${TEST_FILES})
target_link_libraries(tests PRIVATE brainfreeze-interpreter Catch2::Catch2)
target_compile_features(tests PUBLIC cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)

#add_custom_target(tests ALL test-brainfreeze
#                  DEPENDS test-brainfreeze
#                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#                  COMMENT Runs application unit tests)
