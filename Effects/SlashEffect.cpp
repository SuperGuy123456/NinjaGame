#include "SlashEffect.h"
#include <algorithm>
#include <cmath>

SlashEffect::SlashEffect(
    const std::vector<Texture2D>* frames,
    Vector2 origin,
    float speed,
    float scale,
    float duration,
    SlashType type,
    int count
)
    : textures(frames)
{
    int frameCount = (int)frames->size();

    auto makeSlash = [&](float angleDeg) {
        SlashParticle p;

        p.pos = origin;
        p.rotation = angleDeg;
        p.age = 0;
        p.duration = duration;
        p.scale = scale;
        p.squish = 1.0f;
        p.frameIndex = GetRandomValue(0, frameCount - 1);

        float rad = angleDeg * DEG2RAD;
        p.vel = { cosf(rad) * speed, sinf(rad) * speed };

        particles.push_back(p);
    };

    switch (type) {
        case SLASH_ONE_SIDE:
            for (int i = 0; i < count; i++)
                makeSlash(0);
            break;

        case SLASH_DOUBLE_SIDE:
            for (int i = 0; i < count; i++) {
                makeSlash(0);
                makeSlash(180);
            }
            break;

        case SLASH_UNIFORM:
            for (int i = 0; i < count; i++) {
                float angle = (360.0f / count) * i;
                makeSlash(angle);
            }
            break;
    }
}


void SlashEffect::Update(float dt) {
    if (particles.empty()) {
        dead = true;
        return;
    }

    for (auto& p : particles) {
        p.age += dt;

        // Start squishing after duration
        if (p.age >= p.duration) {
            p.squish -= dt * 2.0f; // shrink quickly
            if (p.squish < 0) p.squish = 0;
        }

        // Move outward
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
    }

    // Remove dead slashes
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](auto& p){ return p.squish <= 0; }),
        particles.end()
    );

    if (particles.empty())
        dead = true;
}

void SlashEffect::Draw() {
    for (auto& p : particles) {
        const Texture2D& tex = (*textures)[p.frameIndex];

        float w = tex.width * p.scale;
        float h = tex.height * p.scale * p.squish; // squish vertically

        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle dest = { p.pos.x, p.pos.y, w, h };
        Vector2 origin = { w / 2, h / 2 };

        DrawTexturePro(tex, src, dest, origin, p.rotation, WHITE);
    }
}
