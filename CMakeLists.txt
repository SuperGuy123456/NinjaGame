cmake_minimum_required(VERSION 3.16)
project(NinjaGame)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

add_subdirectory(external/raylib)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.1
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

FetchContent_Declare(
        rlimgui
        GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
        GIT_TAG 286e11acd6c785004c9550c7ed3762add2ae3d47
)
FetchContent_MakeAvailable(rlimgui)

add_library(rlimgui STATIC
        ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)
target_include_directories(rlimgui PUBLIC ${rlimgui_SOURCE_DIR})
target_link_libraries(rlimgui PUBLIC raylib imgui)

FetchContent_Declare(
        imguifiledialog
        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
        GIT_TAG v0.6.8
)
FetchContent_MakeAvailable(imguifiledialog)

add_library(imguifiledialog STATIC
        ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp
)
target_include_directories(imguifiledialog PUBLIC ${imguifiledialog_SOURCE_DIR})
target_link_libraries(imguifiledialog PUBLIC imgui)

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
        Engine/Chunks.h
        Engine/Animation.cpp
        Engine/Animation.h
        Engine/Collision.cpp
        Engine/Collision.h
        NPC/Dummy.cpp
        NPC/Dummy.h
        Effects/PhysicsBasedParticle.cpp
        Effects/PhysicsBasedParticle.h
        Effects/SlashEffect.cpp
        Effects/SlashEffect.h
        Engine/GameCamera.cpp
        Engine/GameCamera.h
        World/Grass.cpp
        World/Grass.h
        Engine/ReferencePool.cpp
        Engine/ReferencePool.h
        Effects/EffectsManager.cpp
        Effects/EffectsManager.h
        Effects/DEFAULTEFFECTPARAMS.h
        Engine/SoundSystem.cpp
        Engine/SoundSystem.h
        LevelEditor.cpp
        LevelEditor.h)

target_link_libraries(NinjaGame raylib rlimgui imguifiledialog)
