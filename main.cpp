//
// Created by tyagi on 3/1/2026.
//

#include "EventManager.h"
#include "Engine/EventManager.h"
#include "Engine/DrawingPipeline.h"
#include "raylib.h"
#include "Player.h"
#include "screenres.h"
#include "Engine/Chunks.h"
#include "NPC/Dummy.h"
#include "Engine/GameCamera.h"
#include <mach/mach.h>
#include <iostream>
#include "Engine/ReferencePool.h"

using namespace std;

#include <mach/mach.h>

void PrintMemoryStats() {
    // --- CPU RAM ---
    mach_task_basic_info basicInfo;
    mach_msg_type_number_t basicCount = MACH_TASK_BASIC_INFO_COUNT;

    task_info(mach_task_self(),
              MACH_TASK_BASIC_INFO,
              (task_info_t)&basicInfo,
              &basicCount);

    double ramMB = (double)basicInfo.resident_size / (1024.0 * 1024.0);

    // --- Unified Memory (CPU + GPU) ---
    task_vm_info_data_t vmInfo;
    mach_msg_type_number_t vmCount = TASK_VM_INFO_COUNT;

    task_info(mach_task_self(),
              TASK_VM_INFO,
              (task_info_t)&vmInfo,
              &vmCount);

    double unifiedMB = (double)vmInfo.phys_footprint / (1024.0 * 1024.0);

    // --- Draw ---
    char buffer[128];
    snprintf(buffer, sizeof(buffer),
             "RAM: %.1f MB\nVRAM/Unified: %.1f MB",
             ramMB, unifiedMB);

    DrawText(buffer, 0, 25, 20, DARKGREEN);
}





int main() {

    std::cout << "[BOOT] InitWindow...\n";
    InitWindow(SCREENWIDTH ,SCREENHEIGHT, "Ninja Game");
    double dt;
    ReferencePool::Add("DELTATIME REF", &dt);
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int windowWidth = (int)(screenWidth);
    int windowHeight = (int)(screenHeight);

    std::cout << "[BOOT] SetWindowPosition...\n";
    SetWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    InitAudioDevice();

    std::cout << "[BOOT] SetTargetFPS...\n";
    SetTargetFPS(60);

    // ---------------- Drawing Pipeline ----------------
    std::cout << "[BOOT] Creating drawing pipeline...\n";
    DrawingPipeline pipeline;
    DrawLayer bglayer;
    DrawLayer tilelayer;
    DrawLayer entitylayer;
    DrawLayer misclayer;
    DrawLayer uilayer;
    ReferencePool::Add("DRAWINGPIPELINE REF", &pipeline);
    ReferencePool::Add("BGLAYER REF", &bglayer);
    ReferencePool::Add("TILELAYER REF", &tilelayer);
    ReferencePool::Add("ENTITYLAYER REF", &entitylayer);
    ReferencePool::Add("MISCLAYER REF", &misclayer);
    ReferencePool::Add("UILAYER REF", &uilayer);

    std::cout << "[BOOT] Adding layers...\n";
    pipeline.AddLayer(&bglayer, "BACKGROUND LAYER");
    pipeline.AddLayer(&tilelayer, "TILE LAYER");
    pipeline.AddLayer(&entitylayer, "ENTITY LAYER");
    pipeline.AddLayer(&misclayer, "MISC LAYER");
    pipeline.AddLayer(&uilayer, "UI LAYER");

    // ---------------- Event Managers ----------------
    std::cout << "[BOOT] Creating event managers...\n";
    EventManager keyboardmanager;
    EventManager playerposmanager;
    InputManager inputmanager(keyboardmanager);
    ReferencePool::Add("KEYBOARDMANAGER REF", &keyboardmanager);
    ReferencePool::Add("PLAYERPOSMANAGER REF", &playerposmanager);
    ReferencePool::Add("INPUTMANAGER", &inputmanager);

    std::cout << "[BOOT] Creating ChunkManager...\n";
    ChunkManager chunkmanager(tilelayer, playerposmanager);
    ReferencePool::Add("CHUNKMANAGER REF", &chunkmanager);

    // ---------------- Player ----------------
    std::cout << "[BOOT] Creating player...\n";
    Player player(Vector2{100,100}, entitylayer, keyboardmanager, playerposmanager, &chunkmanager);
    ReferencePool::Add("PLAYER REF", &player);

    std::cout << "[BOOT] ChunkManager constructed successfully.\n";

    // ---------------- Camera ----------------
    std::cout << "[BOOT] Setting up camera...\n";
    GameCamera::Init(&player.pos);

    std::cout << "[BOOT] Entering main loop...\n";

    Dummy dummy({196.707, 732.234}, entitylayer, playerposmanager, chunkmanager);

    // ---------------- Main Loop ----------------
    while (!WindowShouldClose()) {

        Vector2 mouseScreen = GetMousePosition();
        Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, GameCamera::GetCamera());

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            std::cout << "Mouse World Pos: "
                      << mouseWorld.x << ", "
                      << mouseWorld.y << std::endl;
        }

        dt = GetFrameTime();

        inputmanager.GetInput();
        player.Update(dt);
        chunkmanager.Update();
        dummy.Update(dt);

        // ---------------- Camera Update ----------------
        GameCamera::Update(dt);

        // ---------------- Drawing ----------------
        BeginDrawing();
        ClearBackground(Color{135, 206, 235});
        DrawFPS(0,0);
        PrintMemoryStats();
        chunkmanager.Draw();
        BeginMode2D(GameCamera::GetCamera());
        pipeline.DrawAll();
        EndMode2D();

        EndDrawing();
    }

    std::cout << "[BOOT] Exiting cleanly.\n";
    CloseWindow();
    return 0;
}
