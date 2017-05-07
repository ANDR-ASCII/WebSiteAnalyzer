cmake_minimum_required(VERSION 2.8.11)

set(AppName WebSiteAnalyzer)
set(GuiBinName ${AppName})

project(${AppName})

# lib names
set(CommonLibName Common)
set(CrawlerLibName Crawler)
set(HtmlParserLibName HtmlParser)
set(HttpLibName HttpLib)

# change it if you want to build project on Linux or MAC OS
set(NetworkOSLibName ws2_32)

add_subdirectory(${CommonLibName})
add_subdirectory(${CrawlerLibName})
add_subdirectory(${GuiBinName})
add_subdirectory(${HtmlParserLibName})
add_subdirectory(${HttpLibName})