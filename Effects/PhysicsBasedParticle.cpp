#include "PhysicsBasedParticle.h"
#include <algorithm>
#include <cmath>

PhysicsBasedParticle::PhysicsBasedParticle(
    const std::vector<Texture2D>* frames,
    int particleCount,
    Vector2 origin,
    Vector2 direction,
    ParticleShape shape,

    float minSpeed,
    float maxSpeed,

    float minLifetime,
    float maxLifetime,

    float minRotSpeed,
    float maxRotSpeed,

    float scale,
    float gravityVal,
    float dragVal,

    ChunkManager* chunk
)
    : textures(frames),
      chunkmanager(chunk),
      gravity(gravityVal),
      drag(dragVal)
{
    particles.reserve(particleCount);

    int frameCount = (int)textures->size();

    for (int i = 0; i < particleCount; i++) {
        Particle p;

        p.pos = origin;
        p.scale = scale;
        p.frameIndex = GetRandomValue(0, frameCount - 1);

        // Rotation
        p.rotation = (float)GetRandomValue(0, 360);
        p.rotationSpeed = (float)GetRandomValue((int)minRotSpeed, (int)maxRotSpeed);

        // Lifetime
        p.lifetime = minLifetime + (float)GetRandomValue(0, 1000) / 1000.0f * (maxLifetime - minLifetime);

        float speed = minSpeed + (float)GetRandomValue(0, 1000) / 1000.0f * (maxSpeed - minSpeed);

        switch (shape) {
            case UNIFORM_EXPLOSION: {
                float angle = GetRandomValue(0, 360) * DEG2RAD;
                p.vel = { cosf(angle) * speed, sinf(angle) * speed };
                break;
            }

            case DIRECTIONAL_BURST: {
                float angleOffset = GetRandomValue(-30, 30) * DEG2RAD;
                float baseAngle = atan2f(direction.y, direction.x);
                float angle = baseAngle + angleOffset;
                p.vel = { cosf(angle) * speed, sinf(angle) * speed };
                break;
            }

            case FOUNTAIN_BURST: {
                float angle = GetRandomValue(-20, 20) * DEG2RAD;
                float baseAngle = -90 * DEG2RAD;
                float finalAngle = baseAngle + angle;
                p.vel = { cosf(finalAngle) * speed, sinf(finalAngle) * speed };
                break;
            }
        }

        p.collider = { p.pos.x, p.pos.y, 2, 2 };
        particles.push_back(p);
    }
}

void PhysicsBasedParticle::Update(float dt) {
    if (particles.empty()) {
        dead = true;
        return;
    }

    for (auto& p : particles) {
        p.age += dt;

        if (p.age >= p.lifetime) {
            p.alpha -= dt * 1.0f;
            if (p.alpha < 0) p.alpha = 0;
        }

        p.vel.y += gravity * dt;
        p.vel.x *= drag;

        Vector2 nextPos = { p.pos.x + p.vel.x * dt, p.pos.y + p.vel.y * dt };
        Rectangle nextBox = { nextPos.x, nextPos.y, 2, 2 };

        // --- UPDATED COLLISION CHECK ---
        if (chunkmanager) {
            int tileID = chunkmanager->CheckTileCollision({nextPos.x, nextPos.y});

            // If the tile is solid (not 0) AND it's not Grass (not 3)
            if (tileID != 0 && tileID != 3) {
                p.vel.y *= -0.3f; // Bounce
                p.vel.x *= 0.7f; // Friction

                if (fabs(p.vel.y) < 20)
                    p.vel.y = 0;
            } else {
                // If it's air (0) OR grass (3), move through it freely
                p.pos = nextPos;
            }
        } else {
            p.pos = nextPos;
        }

        p.collider.x = p.pos.x;
        p.collider.y = p.pos.y;

        p.rotation += p.rotationSpeed * dt;
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p){ return p.alpha <= 0; }),
        particles.end()
    );
    particles.shrink_to_fit();


    if (particles.empty())
        dead = true;
}

void PhysicsBasedParticle::Draw() {
    for (auto& p : particles) {
        const Texture2D& tex = (*textures)[p.frameIndex];

        Color tint = WHITE;
        tint.a = (unsigned char)(p.alpha * 255);

        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle dest = { p.pos.x, p.pos.y, tex.width * p.scale, tex.height * p.scale };
        Vector2 origin = { (tex.width * p.scale) / 2, (tex.height * p.scale) / 2 };

        DrawTexturePro(tex, src, dest, origin, p.rotation, tint);
    }
}
