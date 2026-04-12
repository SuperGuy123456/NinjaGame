//
// Created by Manomay Tyagi on 4/11/26.
//

#ifndef NINJAGAME_EFFECTSMANAGER_H
#define NINJAGAME_EFFECTSMANAGER_H

#include "Effects/SlashEffect.h"
#include "Effects/PhysicsBasedParticle.h"
#include "raylib.h"
#include "Engine/DrawingPipeline.h"
#include "Engine/ReferencePool.h"
#include "BaseClasses/Base.h"
#include "Effects/DEFAULTEFFECTPARAMS.h"

class EffectsManager {
public:
    static void Init(); //load ALL effect images
    static void DeInit(); // unload all

    static void SpawnPBPEffect(Vector2 spawnPos);
    static void SpawnPBPEffect(const std::vector<Texture2D>* frames,   // <-- pointer to external textures
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
    static void SpawnSlashEffect(Vector2 spawnPos);
    static void SpawnSlashEffect(const std::vector<Texture2D>* frames,
    Vector2 origin,
    float speed,
    float scale,
    float duration,
    SlashType type,
    int count);

    static void Draw();
    static void Update();
private:
    static vector<Effect*> currentEffects;
    static vector<Texture2D> hayeffect;
    static vector<Texture2D> slasheffect;
};


#endif //NINJAGAME_EFFECTSMANAGER_H