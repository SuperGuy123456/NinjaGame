#include "Grass.h"
#include "Player.h"

Grass::Grass(Vector2& _pos) : pos(_pos), player((Player*)ReferencePool::Get("PLAYER REF")){

    int localX = 0;
    for (int i = 0; i < count + 1; i++) {
        int height = GetRandomValue(min_height, max_height);

        Blade blade = {
            Vector2{ pos.x + localX, pos.y + height + 5 }, // Base
            Vector2{ pos.x + localX, pos.y + 5 },          // Tip
            grassPalette[GetRandomValue(0, 3)]
        };

        blades.push_back(blade);
        localX += spacing;
    }

}

Grass::~Grass() {
}

void Grass::Draw() {
    for (const auto& blade : blades) {
        // Use DrawLineEx if you want a specific thickness (like 1.0f or 1.5f)
        DrawLineV(blade.base, blade.tip, blade.color);
    }
}

void Grass::Update() {
    Vector2 playerpos = player->pos;
    float dt = GetFrameTime();

    for (auto& blade : blades) {
        float dist = playerpos.x - blade.base.x;
        float targetXOffset = 0.0f;

        if (fabs(dist) < 5.0f) {
            float force = 0.8f - (fabs(dist) / 5.0f);
            targetXOffset = (dist > 0 ? -1.0f : 1.0f) * force * 10.0f;
        }

        float targetX = blade.base.x + targetXOffset;
        blade.tip.x = Lerp(blade.tip.x, targetX, dt * 10.0f);

        // Squish factor
        float dx = blade.tip.x - blade.base.x;
        float squish = (dx * dx) / 50.0f;
        blade.tip.y = Lerp(blade.tip.y, (blade.base.y - max_height) + squish, dt * 10.0f);
    }
}