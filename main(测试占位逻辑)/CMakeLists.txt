cmake_minimum_required(VERSION 4.0)
project(MudOUC)

set(CMAKE_CXX_STANDARD 20)

add_executable(MudOUC main.cpp
        game.cpp
        game.h
        room.cpp
        room.h
        ../../Desktop/MudOUC/Player.h
        ../../Desktop/MudOUC/Player.cpp)
