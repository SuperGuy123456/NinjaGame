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

using namespace std;
//ENTIRE GAME RUNS ON PIXEL ART WITH SCALE OF 10
int main() {


    std::cout << "[BOOT] InitWindow...\n";
    InitWindow(SCREENWIDTH ,SCREENHEIGHT, "Ninja Game");

    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int windowWidth = (int)(screenWidth);
    int windowHeight = (int)(screenHeight);

    std::cout << "[BOOT] SetWindowPosition...\n";
    SetWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);

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

    // ---------------- Player ----------------
    std::cout << "[BOOT] Creating player...\n";
    Player player(Vector2{100,100}, entitylayer, keyboardmanager, playerposmanager);

    std::cout << "[BOOT] Creating ChunkManager...\n";
    ChunkManager chunkmanager(bglayer, playerposmanager);

    std::cout << "[BOOT] ChunkManager constructed successfully.\n";

    // ---------------- Camera ----------------
    std::cout << "[BOOT] Setting up camera...\n";
    Camera2D camera = { 0 };
    camera.target = Vector2{ 100, 100 };
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.1f;

    std::cout << "[BOOT] Entering main loop...\n";

    // ---------------- Main Loop ----------------
    while (!WindowShouldClose()) {
        double dt = GetFrameTime();

        camera.target = player.pos;

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(Color{135, 206, 235});

        inputmanager.GetInput();
        player.Update(dt);
        chunkmanager.Update();
        pipeline.DrawAll();

        EndMode2D();
        EndDrawing();
    }

    std::cout << "[BOOT] Exiting cleanly.\n";
    CloseWindow();
    return 0;
}
