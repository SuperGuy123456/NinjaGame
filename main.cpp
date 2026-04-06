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

    std::cout << "[BOOT] Creating ChunkManager...\n";
    ChunkManager chunkmanager(bglayer, playerposmanager);
    // ---------------- Player ----------------
    std::cout << "[BOOT] Creating player...\n";
    Player player(Vector2{100,100}, entitylayer, keyboardmanager, playerposmanager, &chunkmanager);


    std::cout << "[BOOT] ChunkManager constructed successfully.\n";

    // ---------------- Camera ----------------
    std::cout << "[BOOT] Setting up camera...\n";
    Camera2D camera = { 0 };
    camera.target = Vector2{ 100, 100 };
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    std::cout << "[BOOT] Entering main loop...\n";

    SetTargetFPS(60);

    // ---------------- Main Loop ----------------
    while (!WindowShouldClose()) {
        double dt = GetFrameTime();

        inputmanager.GetInput();
        player.Update(dt);
        chunkmanager.Update();

        // ---------------- Camera Update (AFTER player moves) ----------------
        float lerp = 0.15f;
        float targetX = camera.target.x + (player.pos.x - camera.target.x) * lerp;
        float targetY = camera.target.y + (player.pos.y - camera.target.y) * lerp;

        targetX = floor(targetX);
        targetY = floor(targetY);

        camera.target.x = targetX;
        camera.target.y = targetY;

        camera.offset.x = floor(camera.offset.x);
        camera.offset.y = floor(camera.offset.y);

        // ---------------- Drawing ----------------
        BeginDrawing();
        ClearBackground(Color{135, 206, 235});
        DrawFPS(0,0);
        BeginMode2D(camera);
        pipeline.DrawAll();

        EndMode2D();
        EndDrawing();
    }


    std::cout << "[BOOT] Exiting cleanly.\n";
    CloseWindow();
    return 0;
}
