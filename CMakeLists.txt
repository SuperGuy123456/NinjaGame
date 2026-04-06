cmake_minimum_required(VERSION 3.16)
project(NinjaGame)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(external/raylib)

include_directories(.)
include_directories(Engine)

add_executable(NinjaGame
        Engine/Allstructs.h
        Engine/DrawingPipeline.cpp
        Engine/DrawingPipeline.h
        Engine/EventManager.cpp
        Engine/EventManager.h
        Engine/InputManager.cpp
        Engine/InputManager.h
        main.cpp
        BaseClasses/Base.h
        Player.cpp
        Player.h
        Engine/Spritesplitter.cpp
        screenres.h
        Engine/Chunks.cpp
        Engine/Chunks.h)

target_link_libraries(NinjaGame raylib)