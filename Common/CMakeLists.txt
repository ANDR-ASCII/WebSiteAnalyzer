cmake_minimum_required(VERSION 2.8.11)

project(${CommonLibName})

set(CommonLibSourcesDir src)
set(CommonLibHeadersDir include)

aux_source_directory(${CommonLibSourcesDir} CommonLibSourcesList)
file(GLOB_RECURSE CommonLibHeadersList "include/*.h")

if(MSVC)
	add_definitions(/MP)
endif()

include_directories(${CommonLibHeadersDir})

add_library(
	${CommonLibName} STATIC
	${CommonLibSourcesList}
	${CommonLibHeadersList}
)

target_link_libraries(${CommonLibName})