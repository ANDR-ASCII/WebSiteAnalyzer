cmake_minimum_required(VERSION 2.8)

project(${HTMLPARSER_LIB_NAME})

set(HTMLPARSER_LIB_SOURCES_DIR src)

aux_source_directory(${HTMLPARSER_LIB_SOURCES_DIR} HTMLPARSER_LIB_SOURCES_LIST)
file(GLOB_RECURSE HTMLPARSER_LIB_HEADERS_LIST "include/*.h")

# Boost dependencies
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59.0 COMPONENTS regex)

# this project depends of boost binary only
include_directories(
	${HTMLPARSER_LIB_HEADERS_DIR} 
	${Boost_INCLUDE_DIRS}
)
	
add_library(
	${HTMLPARSER_LIB_NAME} STATIC 
	${HTMLPARSER_LIB_SOURCES_LIST} 
	${HTMLPARSER_LIB_HEADERS_LIST}
)

target_link_libraries(
	${HTMLPARSER_LIB_NAME} 
	${Boost_LIBRARIES}
)