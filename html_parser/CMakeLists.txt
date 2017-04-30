cmake_minimum_required(VERSION 2.8)

project(${HTMLPARSER_LIB_NAME})

set(HEADERS_DIR include)
set(SOURCES_DIR src)

aux_source_directory(${SOURCES_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "./include/*.h")

# Boost dependencies
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59.0 COMPONENTS regex)

include_directories(
	${HEADERS_DIR} 
	${HEADERS_LIST} 
	${Boost_INCLUDE_DIRS}
)

add_definitions(
	/D HTML_PARSER_LIBRARY
)
	
add_library(
	${HTMLPARSER_LIB_NAME} STATIC 
	${SOURCES_LIST} 
	${HEADERS_LIST}
)

target_link_libraries(
	${HTMLPARSER_LIB_NAME} 
	${Boost_LIBRARIES}
)