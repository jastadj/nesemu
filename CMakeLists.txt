set(PROJ_NAME "nesemu")
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(${PROJ_NAME} CXX C)

# Set the Build Output directory
if(NOT DEFINED BUILDOUTPUT_DIR)
	set(BUILDOUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()
set(CMAKE_INSTALL_PREFIX ${BUILDOUTPUT_DIR})

add_compile_definitions(
	#GLAD_GLAPI_EXPORT
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLFW Lib
if(NOT DEFINED GLFW_DIR)
	set(GLFW_DIR $ENV{GLFW_DIR})
endif()
if(NOT GLFW_DIR)
	message(FATAL_ERROR "GLFW_DIR is not set!")
endif()
message(STATUS "GLFW_DIR: ${GLFW_DIR}")
find_library(GLFW
    NAMES glfw3.lib
    HINTS "${GLFW_DIR}/lib"
	REQUIRED
	NO_DEFAULT_PATH
)

file(GLOB_RECURSE PROJ_SOURCE
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_SOURCE_DIR}/glad/src/*.c"
)

file(GLOB_RECURSE PROJ_HEADERS
	"${CMAKE_SOURCE_DIR}/include/*.h"
	"${CMAKE_SOURCE_DIR}/glad/include/glad/*.h"
	
)

add_executable(${PROJ_NAME} ${PROJ_SOURCE} ${PROJ_HEADERS})

target_include_directories(${PROJ_NAME}
	PRIVATE
		"${CMAKE_SOURCE_DIR}/include"
		"${CMAKE_SOURCE_DIR}/glad/include"
		"${GLFW_DIR}/include"
)

target_link_libraries(${PROJ_NAME}
	PRIVATE
		${GLFW}
		OpenGL::GL		
)

# Install
install(TARGETS ${PROJ_NAME}
	PUBLIC_HEADER
)
install(FILES $<TARGET_RUNTIME_DLLS:${PROJ_NAME}> TYPE BIN)
