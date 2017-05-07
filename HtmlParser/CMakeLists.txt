cmake_minimum_required(VERSION 2.8.11)

project(${HtmlParserLibName})

set(HtmlParserLibSourcesDir src)
set(HtmlParserLibHeadersDir include)

# deps
set(CommonLibHeadersDir ../Common/include)

aux_source_directory(${HtmlParserLibSourcesDir} HtmlParserLibSourcesList)
file(GLOB_RECURSE HtmlParserLibHeadersList "include/*.h")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.59.0 COMPONENTS regex)

include_directories(
	${HtmlParserLibHeadersDir}
	${CommonLibHeadersDir}
	${Boost_INCLUDE_DIRS}
)
	
add_library(
	${HtmlParserLibName} STATIC 
	${HtmlParserLibSourcesList} 
	${HtmlParserLibHeadersList}
)

target_link_libraries(
	${HtmlParserLibName}
	${CommonLibName}
	${Boost_LIBRARIES}
)