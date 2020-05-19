# Author: Scott MacDonald
# CMake module for C++ ASAN/TSAN/UBSAN.
# -----------------------------------------------------------------------------
# NOTE: Only use one sanitizer per build output. Results are best when all
#       targets are built with the sanitizer. Do not mix and match.
find_program(
    CLANG_TIDY_EXE NAMES "clang-tidy"
    DOC "Path to clang-tidy executable")

if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    set(HAS_CLANG_TIDY)
    set(CLANG_TIDY_PROPERTIES "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-cplusplus")
endif()