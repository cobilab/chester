cmake_minimum_required(VERSION 2.8.4)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
IF(UNIX)
 link_libraries(m)
ENDIF(UNIX)

project (CHESTER) 
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
SET(CMAKE_BUILD_TYPE Release)

add_executable (CHESTER-map chester-map.c mem.c msg.c model.c paint.c common.c parser.c buffer.c defs.h) 
add_executable (CHESTER-filter chester-filter.c msg.c mem.c filters.c segment.c common.c parser.c buffer.c defs.h) 
add_executable (CHESTER-visual chester-visual.c msg.c mem.c paint.c common.c parser.c buffer.c defs.h) 
TARGET_LINK_LIBRARIES(CHESTER-map)
TARGET_LINK_LIBRARIES(CHESTER-filter)
TARGET_LINK_LIBRARIES(CHESTER-visual)
