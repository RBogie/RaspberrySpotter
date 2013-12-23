cmake_minimum_required(VERSION 2.8)

project(RaspberrySpotter)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules" ${CMAKE_MODULE_PATH})

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

find_package(libspotify REQUIRED)
find_package (Threads REQUIRED)
find_package(ALSA REQUIRED)

if(LIBSPOTIFY_FOUND)
	link_directories ( ${LIBSPOTIFY_LIBRARY_DIRS} )
	include_directories ( ${LIBSPOTIFY_INCLUDE_DIRS} )
endif()
if(ALSA_FOUND)
	link_directories ( ${ALSA_LIBRARY_DIRS} )
	include_directories ( ${ALSA_INCLUDE_DIRS} )
endif()

add_subdirectory(Projects)
