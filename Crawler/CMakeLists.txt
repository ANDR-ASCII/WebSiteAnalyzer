cmake_minimum_required(VERSION 2.8.11)

project(${CrawlerLibName})

set(CrawlerLibSourcesDir src)
set(CrawlerLibHeadersDir include)

# .lib headers directories
set(HtmlParserLibHeadersDir ../HtmlParser/include)
set(HttpLibHeadersDir ../HttpLib/include)
set(CommonLibHeadersDir ../Common/include)
set(GumboLibHeadersDir ../Gumbo/include)

# prepare headers directories and sources list
set(AllHeadersDir
	${GumboLibHeadersDir}
	${CrawlerLibHeadersDir} 
	${HtmlParserLibHeadersDir}
	${CommonLibHeadersDir}
	${HttpLibHeadersDir}
)

aux_source_directory(${CrawlerLibSourcesDir} CrawlerLibSourcesList)
file(GLOB_RECURSE CrawlerLibHeadersList "include/*.h")

set(AllSources 
	${CrawlerLibSourcesList}
)

set(AllHeaders 
	${CrawlerLibHeadersList}
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64.0 COMPONENTS regex)

if(MSVC)
	add_definitions(/MP)
endif()

include_directories(
	${AllHeadersDir}
	${Boost_INCLUDE_DIRS}
)

add_library(
	${CrawlerLibName} STATIC
	${AllSources} 
	${AllHeaders}
)

target_link_libraries(
	${CrawlerLibName} 
	${Boost_LIBRARIES}
	${CommonLibName}
	${HtmlParserLibName}
	${HttpLibName}
	${GumboLibName}
)