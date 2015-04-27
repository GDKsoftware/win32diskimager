cmake_minimum_required(VERSION 2.8.12)

project(win32diskimager)

add_definitions(-std=c++11)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DCONSOLE -D_CONSOLE)

set(cli_SOURCES
	disk.cpp
	deviceinfo.cpp
	diskwriter.cpp
	ui_helper.cpp
	cli_main.cpp)

add_executable(win32diskimager ${cli_SOURCES})


remove_definitions(-DCONSOLE -D_CONSOLE)
add_definitions(-DWIN)
add_definitions(-DWIN32)
add_definitions(-D_WINDOWS)
add_definitions(-D_WINDLL)
add_definitions(-D_USRDLL)
add_definitions(-DWIN32DISKIMAGERDLL_EXPORTS)

set(dll_SOURCES
	disk.cpp
	deviceinfo.cpp
	diskwriter.cpp
	ui_helper.cpp
	Win32DiskImagerDLL.cpp
	dll_main.cpp)

SET (LIB_TYPE SHARED)
add_library(win32diskimagerdll ${LIB_TYPE} ${dll_SOURCES})
