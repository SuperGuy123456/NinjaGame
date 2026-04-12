#ifndef GRASS_H
#define GRASS_H

#include "raylib.h"
#include <iostream>
#include "ReferencePool.h"
#include "Engine/DrawingPipeline.h"
#include "screenres.h"
#include "Engine/GameCamera.h"
#include "raymath.h"
class Player;

struct Blade {
    Vector2 base;
    Vector2 tip;

    Color color;
};

class Grass : public Load{
public:
    Grass(Vector2& _pos);
    ~Grass();

    void Draw();
    void Update();
private:
    Vector2& pos;
    Player* player;

    const int max_height = 6;
    const int min_height = 3;

    const int max_tipoffset = 2;
    const int min_tipoffset = -2;

    const int width = 1;

    const int spacing = 1;

    const int count = 7;

    const float max_bend = 0.5f;

    const std::vector<Color> grassPalette = {
        {117, 167, 67, 255},  // 1. Lighter Green (Highlights/Sun-kissed)
        {75, 105, 47, 255},   // 2. Your Base Color (The Middle)
        {52, 72, 32, 255},    // 3. Darker Green (Deep Grass)
        {30, 45, 50, 255}     // 4. Cool Dark Green (Shadows/Deepest depth)
    };

    vector<Blade> blades;
};
#endif