cmake_minimum_required(VERSION 3.15)
project(BrainFreeze
  VERSION 0.2.0
  DESCRIPTION "A cross platform Brainfuck interpreter with configurable implementation standards"
  HOMEPAGE_URL "https://github.com/smacdo/brainfreeze")

# Third party dependencies.
add_subdirectory(thirdparty)

# Set warnings as errors, and set the warning level high.
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(src)
add_subdirectory(tests)

# CPack configuration
if (WIN32)
  set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
  set(CPACK_GENERATOR "ZIP;DragNDrop")
else()
  set(CPACK_GENERATOR "STGZ;TGZ;DEB;RPM")
endif()

set(CPACK_PACKAGE_VENDOR "Scott MacDonald")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scott's personal coding sandbox")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

include(CPack)