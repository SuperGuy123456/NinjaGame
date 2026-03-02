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
    InitWindow(1920,1080, "Ninja Game");

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

    Player player(Vector2{0,0}, entitylayer);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}