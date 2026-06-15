set(PROJ_NAME "arch6502")
set(PROJ_NAMESPACE ${PROJ_NAME})

# Project Source
set(PROJ_SOURCE
	src/asm6502.cpp
	src/cpu6502.cpp
	src/cpu6502clock.cpp
	src/cpuclock.cpp
	src/defs6502.cpp
	src/memorybank.cpp
	src/memorymap.cpp
	src/opcodes6502.cpp
	src/submemorymap.cpp
	src/tools.cpp
	
)

set(PROJ_HEADERS
)

set(PROJ_PUBLIC_HEADERS
	include/asm6502.h
	include/cpu6502.h
	include/cpu6502clock.h
	include/cpuclock.h
	include/defs6502.h
	include/memorybank.h
	include/memorymap.h
	include/opcodes6502.h
	include/submemorymap.h
	include/tools.h
)

add_library(${PROJ_NAME} STATIC ${PROJ_SOURCE} ${PROJ_HEADERS} ${PROJ_PUBLIC_HEADERS})
add_library(${PROJ_NAMESPACE}::${PROJ_NAME} ALIAS ${PROJ_NAME})

target_include_directories(${PROJ_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJ_NAME}
	PRIVATE
)

# Install
install(TARGETS ${PROJ_NAME}
    EXPORT ${PROJ_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	PUBLIC_HEADER
)

