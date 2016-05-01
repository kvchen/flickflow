cmake_minimum_required(VERSION 2.8)

# FlickFlow source
set(FLICKFLOW_SOURCE
    fluidDynamics.cpp
    main.cpp
    slab.cpp
)

# FlickFlow header
set(FLICKFLOW_HEADER
    slab.h
)


#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable( flickflow
    ${FLICKFLOW_SOURCE}
    ${FLICKFLOW_HEADER}
)

target_link_libraries( flickflow
    glew ${GLEW_LIBRARIES}
    glfw3 ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET flickflow APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)


# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS flickflow DESTINATION ${FlickFlow_SOURCE_DIR})
