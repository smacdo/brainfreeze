find_package( Boost 1.38 COMPONENTS program_options REQUIRED )
include_directories( . ${Boost_INCLUDE_DIR} )

set(SOURCES bf.cpp bfutils.cpp cli.cpp)

add_executable(brainfreeze ${SOURCES})
TARGET_LINK_LIBRARIES(brainfreeze ${Boost_LIBRARIES})
