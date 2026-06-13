set(PROJ_NAME "nesemu")
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(${PROJ_NAME} CXX C)
set(CMAKE_CXX_STANDARD 17)

# Set the Build Output directory
if(NOT DEFINED BUILDOUTPUT_DIR)
	set(BUILDOUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()
set(CMAKE_INSTALL_PREFIX ${BUILDOUTPUT_DIR})

# Dependency Paths
if(NOT DEFINED GLFW_PATH)
	set(GLFW_PATH $ENV{GLFW_PATH} CACHE PATH "Path to GLFW")
endif()
if(NOT DEFINED GLAD_PATH)
	set(GLAD_PATH $ENV{GLAD_PATH} CACHE PATH "Path to GLAD")
endif()
if(NOT DEFINED GLM_PATH)
	set(GLM_PATH $ENV{GLM_PATH} CACHE PATH "Path to GLM")
endif()
if(NOT DEFINED FREETYPE_PATH)
	set(FREETYPE_PATH $ENV{FREETYPE_PATH} CACHE PATH "Path to FREETYPE")
endif()


# Find OpenGL
find_package(OpenGL REQUIRED)


# GLFW Lib
if(NOT GLFW_PATH)
	message(FATAL_ERROR "GLFW_PATH is not set!")
endif()
message(STATUS "GLFW_PATH: ${GLFW_PATH}")
find_library(GLFW
    NAMES glfw3.lib
    HINTS "${GLFW_PATH}/lib"
	REQUIRED
	NO_DEFAULT_PATH
)

# GLAD
if(NOT GLAD_PATH)
	message(FATAL_ERROR "GLAD_PATH is not set!")
endif()
message(STATUS "GLAD_PATH: ${GLAD_PATH}")

# GLM
if(NOT GLM_PATH)
	message(FATAL_ERROR "GLM_PATH is not set!")
endif()
message(STATUS "GLM_PATH: ${GLM_PATH}")
find_package(glm
	HINTS
		${GLM_PATH}/share/glm
	REQUIRED
	NO_DEFAULT_PATH
)
get_target_property(GLM_INC_DIR glm::glm INTERFACE_INCLUDE_DIRECTORIES)

# FREETYPE
if(NOT FREETYPE_PATH)
	message(FATAL_ERROR "FREETYPE_PATH is not set!")
endif()
message(STATUS "FREETYPE_PATH: ${FREETYPE_PATH}")
find_package(FREETYPE
	HINTS
		${FREETYPE_PATH}/lib/cmake
	REQUIRED
	NO_DEFAULT_PATH
)

# MODULES
add_subdirectory(modules)

# Project Source
file(GLOB_RECURSE PROJ_SOURCE
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${GLAD_PATH}/src/*.c"
)
file(GLOB_RECURSE PROJ_HEADERS
	"${CMAKE_SOURCE_DIR}/include/*.h"
	"${GLAD_PATH}/include/glad/*.h"
)

add_executable(${PROJ_NAME} ${PROJ_SOURCE} ${PROJ_HEADERS})

target_include_directories(${PROJ_NAME}
	PRIVATE
		include
		"${GLAD_PATH}/include"
		"${GLFW_PATH}/include"
		"${GLM_PATH}/include"
		"${FREETYPE_PATH}/include"
)

target_link_libraries(${PROJ_NAME}
	PRIVATE
		${GLFW}
		OpenGL::GL		
		glm::glm
		Freetype::Freetype
)

# Install
install(TARGETS ${PROJ_NAME}
	PUBLIC_HEADER
)
install(FILES $<TARGET_RUNTIME_DLLS:${PROJ_NAME}> TYPE BIN)
