#pragma once

#include "raylib.h"
#include "Engine/Spritesplitter.h"
#include "Engine/Chunks.h"
#include <vector>

enum ParticleShape {
    UNIFORM_EXPLOSION = 1,
    DIRECTIONAL_BURST = 2,
    FOUNTAIN_BURST    = 3
};

struct Particle {
    Vector2 pos;
    Vector2 vel;

    float rotation = 0.0f;
    float rotationSpeed = 0.0f;

    float lifetime = 1.0f;
    float age = 0.0f;

    float alpha = 1.0f;
    float scale = 1.0f;

    int frameIndex = 0;

    Rectangle collider;
};

class PhysicsBasedParticle {
public:
    PhysicsBasedParticle(
        const std::vector<Texture2D>* frames,   // <-- pointer to external textures
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
        float gravity,
        float drag,

        ChunkManager* chunk
    );

    void Update(float dt);
    void Draw();

    bool IsFinished() const { return dead; }

private:
    const std::vector<Texture2D>* textures;  // <-- NOT owned
    std::vector<Particle> particles;

    ChunkManager* chunkmanager;

    float gravity;
    float drag;

    bool dead = false;
};
