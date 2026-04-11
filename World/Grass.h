#ifndef GRASS_H
#define GRASS_H

#include "raylib.h"
#include <iostream>
#include "ReferencePool.h"
#include "Engine/DrawingPipeline.h"
#include "screenres.h"
#include "Engine/GameCamera.h"
class Player;

struct Triangle {
    Vector2 p1;
    Vector2 p2;
    Vector2 p3;

    Vector2 desired_angle; //used to change rotation
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

    const int max_height = 8;
    const int min_height = 4;

    const int max_tipoffset = 4;
    const int min_tipoffset = -4;

    const int width = 2;

    const int spacing = 1;

    const int count = 10;

    const float max_bend = 0.5f;

    const Color basecolor = {75,105,47, 255};

    vector<Triangle> blades;
};
#endif