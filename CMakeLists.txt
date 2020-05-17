cmake_minimum_required(VERSION 3.15)
project(BrainFreeze
  VERSION 0.2.0
  DESCRIPTION "A cross platform Brainfuck interpreter with configurable implementation standards"
  HOMEPAGE_URL "https://github.com/smacdo/brainfreeze")

##### IMPORTANT VALUES TO UPDATE ON VERSION BUMPS #####
# Make sure this is set to a static value for release builds:
#SET(CPACK_WIX_UPGRADE_GUID "")

# Third party dependencies.
add_subdirectory(thirdparty)

# Set warnings as errors, and set the warning level high.
if(MSVC)
	add_compile_options(/W4 /WX)

	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		# Disable "unreferenced formal parameter" warnings in release mode (some referenced only by debug checks).
		add_compile_options(/wd4100)
	endif()  
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)

	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		# Disable "unreferenced formal parameter" warnings in release mode (some referenced only by debug checks).
		add_compile_options(-Wno-unused)
	endif()  
endif()

# Source code and unit tests.
add_subdirectory(src)
add_subdirectory(tests)

# Main program installation.
include(InstallRequiredSystemLibraries)

# CPack configuration
if (WIN32)
  set(CPACK_GENERATOR "ZIP;WIX")
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
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_PROJECT_NAME})
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/wixpatch.xml")

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
)

set(CPACK_WIX_PRODUCT_GUID "9EF095B9-AE36-4497-8A39-CAA62684906F")

include(CPack)