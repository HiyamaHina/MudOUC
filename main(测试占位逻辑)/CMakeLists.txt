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
        CommandParser.cpp
        CommandParser.h
)