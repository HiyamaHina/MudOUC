cmake_minimum_required(VERSION 3.20)
project(MudOUC)

set(CMAKE_CXX_STANDARD 20)

add_executable(MudOUC main.cpp
        game.cpp
        game.h
        room.cpp
        room.h
        ../Player.h
        ../Player.cpp
        ../module5/CommandParser.cpp
        ../module5/CommandParser.h
        ../Item.cpp
        ../SaveManager.cpp
)
target_include_directories(MudOUC PRIVATE ../module5)
if(MSVC)
    target_compile_options(MudOUC PRIVATE /utf-8)
else()
    target_compile_options(MudOUC PRIVATE -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
