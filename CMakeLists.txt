cmake_minimum_required(VERSION 2.8)

set(BINARY_NAME web_site_indexer)

# .lib names
set(HTMLPARSER_LIB_NAME html_parser)

# change it if you want to build project on Linux
set(NETWORKING_WINDOWS_LIB_NAME ws2_32)

##############################################

project(${BINARY_NAME})

set(HEADERS include)
set(SOURCES src)

set(HTMLPARSER_HEADERS html_parser/include)
set(HTMLPARSER_SOURCES html_parser/src)

set(HTTPLIB_HEADERS http_lib/include)
set(HTTPLIB_SOURCES http_lib/src)

set(VIEWHANDLER_HEADERS view_handler/include)
set(VIEWHANDLER_SOURCES view_handler/src)


# prepare headers directories and sources list
set(ALL_HEADERS_DIRS 
	${HEADERS} 
	${HTMLPARSER_HEADERS} 
	${HTTPLIB_HEADERS} 
	${VIEWHANDLER_HEADERS}
)

aux_source_directory(${SOURCES} SOURCES_LIST_MAIN)
aux_source_directory(${HTTPLIB_SOURCES} HTTPLIB_SOURCES_LIST)
aux_source_directory(${VIEWHANDLER_SOURCES} VIEWHANDLER_SOURCES_LIST)

file(GLOB_RECURSE HEADERS_LIST "include/*.h")
file(GLOB_RECURSE HTTPLIB_HEADERS_LIST "http_lib/include/*.h")
file(GLOB_RECURSE VIEWHANDLER_HEADERS_LIST "view_handler/include/*.h")

set(ALL_SOURCES 
	${SOURCES_LIST_MAIN} 
	${HTTPLIB_SOURCES_LIST} 
	${VIEWHANDLER_SOURCES_LIST}
)

set(ALL_HEADERS 
	${HEADERS_LIST} 
	${HTTPLIB_HEADERS_LIST} 
	${VIEWHANDLER_HEADERS_LIST}
)

add_subdirectory(${HTMLPARSER_LIB_NAME})

# Boost dependencies
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59.0 COMPONENTS regex)

include_directories(
	${ALL_HEADERS_DIRS} 
	${Boost_INCLUDE_DIRS}
)

# add macro definitions for Visual Studio
add_definitions(
	/D _WINSOCK_DEPRECATED_NO_WARNINGS
)

add_executable(
	${BINARY_NAME} 
	${ALL_SOURCES} 
	${ALL_HEADERS}
)

target_link_libraries(
	${BINARY_NAME} 
	${Boost_LIBRARIES} 
	${HTMLPARSER_LIB_NAME} 
	${NETWORKING_WINDOWS_LIB_NAME}
)