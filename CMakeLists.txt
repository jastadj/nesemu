set(PROJ_NAME "nesemu")
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(${PROJ_NAME} CXX C)
set(CMAKE_CXX_STANDARD 17)

# Set the Build Output directory
if(NOT DEFINED BUILDOUTPUT_DIR)
	set(BUILDOUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()
set(CMAKE_INSTALL_PREFIX ${BUILDOUTPUT_DIR})

# MODULES
add_subdirectory(modules)

# APP
add_subdirectory(app)


include(GNUInstallDirs)