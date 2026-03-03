//
// Created by Manomay Tyagi on 3/2/26.
//

//
// Created by tyagi on 3/1/2026.
//

#include "EventManager.h"
#include "Engine/EventManager.h"
#include "Engine/DrawingPipeline.h"
#include "raylib.h"
#include "Player.h"

using namespace std;

int main() {
    InitWindow(800, 600, "Ninja Game");

    // Get the current monitor's dimensions
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    // Use 95% of screen size to avoid fullscreen but fill most of the display
    int windowWidth = (int)(screenWidth);
    int windowHeight = (int)(screenHeight);

    // Resize to proper dimensions
    SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2); // Center the window

    DrawingPipeline pipeline;
    DrawLayer bglayer;
    DrawLayer tilelayer;
    DrawLayer entitylayer;
    DrawLayer misclayer;
    DrawLayer uilayer;

    pipeline.AddLayer(&bglayer, "BACKGROUND LAYER");
    pipeline.AddLayer(&tilelayer, "TILE LAYER");
    pipeline.AddLayer(&entitylayer, "ENTITY LAYER");
    pipeline.AddLayer(&misclayer, "MISC LAYER");
    pipeline.AddLayer(&uilayer, "UI LAYER");

    Player player(Vector2{100,100}, entitylayer);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Test: draw a white rectangle to verify drawing works
        DrawRectangle(100, 100, 400, 400, WHITE);

        pipeline.DrawAll();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}