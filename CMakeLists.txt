cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(ConsoleSweeper)
set (CMAKE_CXX_STANDARD 11)

if(MSVC)
	add_definitions(-DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(ConsoleSweeper main.cpp Game.cpp Game.h)
