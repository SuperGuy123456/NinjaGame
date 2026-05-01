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

        // Distance from player to this blade
        float dx = playerpos.x - blade.base.x;
        float dy = playerpos.y - blade.base.y;

        // Reaction thresholds
        const float reactX = 5.0f;   // horizontal range
        const float reactY = 20.0f;  // vertical range

        float targetXOffset = 0.0f;

        // Only react if player is close in BOTH X and Y
        if (fabs(dx) < reactX && fabs(dy) < reactY) {

            // Horizontal force based on closeness
            float force = 0.8f - (fabs(dx) / reactX);
            force = fmaxf(force, 0.0f);

            // Bend away from player
            targetXOffset = (dx > 0 ? -1.0f : 1.0f) * force * 10.0f;
        }

        // Target tip X position
        float targetX = blade.base.x + targetXOffset;

        // Smoothly move tip horizontally
        blade.tip.x = Lerp(blade.tip.x, targetX, dt * 10.0f);

        // Squish factor (vertical bend)
        float bend = blade.tip.x - blade.base.x;
        float squish = (bend * bend) / 50.0f;

        // Smoothly move tip vertically
        blade.tip.y = Lerp(
            blade.tip.y,
            (blade.base.y - max_height) + squish,
            dt * 10.0f
        );
    }
}
