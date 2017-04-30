cmake_minimum_required(VERSION 2.8)

project(${HTTP_LIB_NAME})

set(HTTP_LIB_SOURCES_DIR src)

aux_source_directory(${HTTP_LIB_SOURCES_DIR} HTTP_LIB_SOURCES_LIST)
file(GLOB_RECURSE HTTP_LIB_HEADERS_LIST "include/*.h")

# this project depends of main binary and html parser for common functions
include_directories(
	${HTTP_LIB_HEADERS_DIR}
	${HTMLPARSER_LIB_HEADERS_DIR}
	${BINARY_HEADERS_DIR}
)

add_library(
	${HTTP_LIB_NAME}
	${HTTP_LIB_SOURCES_LIST}
	${HTTP_LIB_HEADERS_LIST}
)

target_link_libraries(
	${HTTP_LIB_NAME}
	${NETWORKING_WINDOWS_LIB_NAME}
)