cmake_minimum_required(VERSION 2.8)
project(FlickFlow)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include_directories("/System/Library/Frameworks")
find_library (COCOA_LIBRARIES Cocoa)
find_library (IOKIT_LIBRARIES IOkit)
find_library (COREVIDEO_LIBRARIES CoreVideo)

find_package(PkgConfig REQUIRED)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)

find_package(GLEW REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(Leap)

add_subdirectory(src)

# Install settings
set(CMAKE_INSTALL_PREFIX "${FlickFlow_SOURCE_DIR}/")
