//
// Created by tyagi on 3/1/2026.
//

#include "EventManager.h"
#include "Engine/EventManager.h"
#include "Engine/DrawingPipeline.h"
#include "raylib.h"
#include "Player.h"
#include "screenres.h"

using namespace std;

int main() {
    // Initialize with a small window first to query monitor size
    InitWindow(SCREENWIDTH ,SCREENHEIGHT, "Ninja Game");

    // Get the current monitor's dimensions
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    // Use 95% of screen size to avoid fullscreen but fill most of the display
    int windowWidth = (int)(screenWidth);
    int windowHeight = (int)(screenHeight);

    // Resize to proper dimensions
    //SetWindowSize(windowWidth, windowHeight);
    SetWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2); // Center the window
    SetTargetFPS(60);

    //drawing pipeline stuff
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

    EventManager keyboardmanager; //where all keyboard events will be broadcasted
    InputManager inputmanager(keyboardmanager);

    //init player
    Player player(Vector2{100,100}, entitylayer, keyboardmanager);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        double dt = GetFrameTime();   // central delta-time

        BeginDrawing();
        ClearBackground(Color{135, 206, 235});

        inputmanager.GetInput();
        player.Update(dt);            // pass dt into player
        pipeline.DrawAll();

        EndDrawing();
    }


    CloseWindow();
    return 0;
}