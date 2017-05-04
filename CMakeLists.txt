cmake_minimum_required(VERSION 2.8)

set(BINARY web_site_indexer)

# .lib names
set(HTMLPARSER_LIB_NAME html_parser)
set(HTTP_LIB_NAME http_lib)
set(GUMBO_PARSER_LIB_NAME gumbo)

# change it if you want to build project on Linux
set(NETWORKING_WINDOWS_LIB_NAME ws2_32)

##############################################

project(${BINARY})

# .h and .cpp binary project directories
set(BINARY_HEADERS_DIR include)
set(BINARY_SOURCES_DIR src)

set(VIEWHANDLER_HEADERS_DIR view_handler/include)
set(VIEWHANDLER_SOURCES_DIR view_handler/src)

# .lib headers directories
set(HTMLPARSER_LIB_HEADERS_DIR html_parser/include)
set(HTTP_LIB_HEADERS_DIR http_lib/include)

set(GUMBO_HEADERS_DIR gumbo_parser/src)

# prepare headers directories and sources list
set(ALL_HEADERS_DIRS 
	${BINARY_HEADERS_DIR} 
	${HTMLPARSER_LIB_HEADERS_DIR}
	${HTTP_LIB_HEADERS_DIR} 
	${VIEWHANDLER_HEADERS_DIR}
	${GUMBO_HEADERS_DIR}
)

aux_source_directory(${BINARY_SOURCES_DIR} BINARY_SOURCES_LIST)
aux_source_directory(${VIEWHANDLER_SOURCES_DIR} VIEWHANDLER_SOURCES_LIST)

file(GLOB_RECURSE BINARY_HEADERS_LIST "include/*.h")
file(GLOB_RECURSE VIEWHANDLER_HEADERS_LIST "view_handler/include/*.h")

set(ALL_SOURCES 
	${BINARY_SOURCES_LIST}
	${VIEWHANDLER_SOURCES_LIST}
)

set(ALL_HEADERS 
	${BINARY_HEADERS_LIST}
	${VIEWHANDLER_HEADERS_LIST}
)

# Boost dependencies
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64.0 COMPONENTS regex)

# this project depends of all subprojects
include_directories(
	${ALL_HEADERS_DIRS} 
	${Boost_INCLUDE_DIRS}
)

# add macro definitions for Visual Studio
if(MSVC)
	add_definitions(
		/D _WINSOCK_DEPRECATED_NO_WARNINGS
	)
endif()

add_subdirectory(${HTMLPARSER_LIB_NAME})
add_subdirectory(${HTTP_LIB_NAME})
add_subdirectory(${GUMBO_PARSER_LIB_NAME})

add_executable(
	${BINARY} 
	${ALL_SOURCES} 
	${ALL_HEADERS}
)

target_link_libraries(
	${BINARY} 
	${Boost_LIBRARIES} 
	${HTMLPARSER_LIB_NAME}
	${HTTP_LIB_NAME}
	${GUMBO_PARSER_LIB_NAME}
)