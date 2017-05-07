cmake_minimum_required(VERSION 2.8.11)

project(${HttpLibName})

set(HttpLibSourcesDir src)
set(HttpLibHeadersDir include)

# deps
set(CrawlerHeadersDir ../Crawler/include)
set(HtmlParserHeadersDir ../HtmlParser/include)
set(CommonLibHeadersDir ../Common/include)

aux_source_directory(${HttpLibSourcesDir} HttpLibSourcesList)
file(GLOB_RECURSE HttpLibHeadersList "include/*.h")

if(MSVC)
	add_definitions(
		/D _WINSOCK_DEPRECATED_NO_WARNINGS
	)
endif()

include_directories(
	${HttpLibHeadersDir}
	${CrawlerHeadersDir}
	${HtmlParserHeadersDir}
	${CommonLibHeadersDir}
)

add_library(
	${HttpLibName}
	${HttpLibSourcesList}
	${HttpLibHeadersList}
)

target_link_libraries(
	${HttpLibName}
	${NetworkOSLibName}
)