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

set(cli_HEADERS
	disk.h
	deviceinfo.h
	diskwriter.h
	ui_helper.h)

add_executable(win32diskimager ${cli_SOURCES})
