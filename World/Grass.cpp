#include "Grass.h"
#include "Player.h"

Grass::Grass(Vector2& _pos) : pos(_pos) {

    int localX = 0;
    for (int i = 0; i < count + 1; i++) {

        int height = GetRandomValue(min_height, max_height);
        int tipoffset = GetRandomValue(min_tipoffset, max_tipoffset);

        Triangle tri = {
            Vector2{ pos.x + localX,         pos.y + height },
            Vector2{ pos.x + localX + width, pos.y + height },
            Vector2{ pos.x + localX + tipoffset + width/2, pos.y },
            Vector2{0,0},
            basecolor
        };

        blades.push_back(tri);
        localX += spacing;
    }
}

Grass::~Grass() {}

void Grass::Draw() {
    // Draw directly in WORLD SPACE.
    // Your main render pipeline + camera will handle scaling and transforms.
    for (Triangle& tri : blades) {
        DrawTriangle(tri.p1, tri.p2, tri.p3, tri.color);
    }
}

void Grass::Update() {
    // nothing for now
}
