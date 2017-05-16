cmake_minimum_required(VERSION 2.8.11)

project(${GumboLibName})

aux_source_directory(./ GUMBO_SOURCES)
file(GLOB_RECURSE GUMBO_HEADERS "./*.h")

add_library(
	${GumboLibName} STATIC 
	${GUMBO_SOURCES} 
	${GUMBO_HEADERS}
)

if(MSVC)

	include_directories(include/)
	
	add_definitions(
		-Dinline=__inline
		/MP
	)
	
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi /GL")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS} /LTCG")
	
endif()