# CMakeLists file
cmake_minimum_required(VERSION 2.6)

project (server)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -Wall -Wno-unknown-pragmas")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -O2")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--hash-style=sysv")

include_directories ("${PROJECT_SOURCE_DIR}/lib/linux/include" "${PROJECT_SOURCE_DIR}/event" )
link_directories ("/usr/local/services/lib")

add_subdirectory (common)
add_subdirectory (event)
add_subdirectory (logic)
add_subdirectory (net)
add_subdirectory (worldd)
add_subdirectory (gamed)
add_subdirectory (countryd)
add_subdirectory (platd)

add_executable (gamed gamed/Daemon.cpp)
add_executable (worldd worldd/worldd.cpp)
add_executable (countryd countryd/country.cpp)
add_executable (platd platd/plat.cpp)

target_link_libraries (gamed game logic event common net protobuf log4cxx config++ tokyotyrant curl)
target_link_libraries (worldd world logic event common net protobuf log4cxx config++ tokyotyrant)
target_link_libraries (countryd country logic event common net protobuf log4cxx config++ tokyotyrant )
target_link_libraries (platd plat logic event common net protobuf log4cxx config++ tokyotyrant)
