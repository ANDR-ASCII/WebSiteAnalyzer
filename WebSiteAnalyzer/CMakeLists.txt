cmake_minimum_required(VERSION 2.8.11)
project(${AppName})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(AppSourcesDir src)
set(AppHeadersDir include)

# deps
set(CommonLibHeadersDir ../Common/include)
set(CrawlerLibHeadersDir ../Crawler/include)
set(HtmlParserLibHeadersDir ../HtmlParser/include)

find_package(Qt5 REQUIRED Widgets)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppHeadersList "include/*.h")
file(GLOB_RECURSE AppFormsList "ui/*.ui")
file(GLOB_RECURSE AppResourcesList "rcc/*.qrc")

qt5_wrap_ui(AppFormsHeadersList ${AppFormsList})
qt5_wrap_cpp(AppHeadersMocList ${AppFormsHeadersList})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64.0 COMPONENTS regex)

add_executable(
	${AppName} 
	${AppSourcesList} 
	${AppHeadersList} 
	${AppFormsList} 
	${AppResourcesList}
	${AppFormsHeadersList}
	${AppHeadersMocList}
)

add_definitions(
	${QT_DEFINITIONS}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES WIN32_EXECUTABLE YES 
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

include_directories(
	${AppHeadersDir}
	${CommonLibHeadersDir}
	${CrawlerLibHeadersDir}
	${HtmlParserLibHeadersDir}
	${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	${AppName}
	${CrawlerLibName}
	${CommonLibName}
	${Boost_LIBRARIES}
	Qt5::Widgets
)