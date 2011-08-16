###########################################################################
### Project tests build script                                          ###
###  - This portion of the build will compile all of the project's unit ###
###    tests, link them into the google test runner and produce a       ###
###    simple command line binary that runs the tests.                  ###
###########################################################################
set(test_srcs test_basics.cpp)

# Required #include paths
include_directories( ${PROJECT_SOURCE_DIR}/src
                     ${GTEST_ROOT}/include )

# Link the executable
add_executable(test-brainfreeze
               ${test_srcs}
               ${PROJECT_SOURCE_DIR}/src/bf.cpp
               ${GTEST_ROOT}/googletestrunner.cpp )

target_link_libraries(test-brainfreeze gtest)

add_custom_target(tests ALL test-brainfreeze
                  DEPENDS test-brainfreeze
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                  COMMENT Runs application unit tests)
