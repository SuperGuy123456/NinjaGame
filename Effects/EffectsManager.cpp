//
// Created by Manomay Tyagi on 4/11/26.
//

#include "EffectsManager.h"

std::vector<Effect*> EffectsManager::currentEffects;
std::vector<Texture2D> EffectsManager::hayeffect;
std::vector<Texture2D> EffectsManager::slasheffect;

void EffectsManager::Init() {
    currentEffects.clear();
    hayeffect.clear();
    slasheffect.clear();
    currentEffects.shrink_to_fit();
    slasheffect.shrink_to_fit();
    currentEffects.clear();

    hayeffect = SpriteSplitter::SplitByHorizontal("../Art/Particles/Hay.png", 3, 0.8);
    slasheffect = SpriteSplitter::SplitByHorizontal("../Art/Particles/WhiteSlash.png", 3, 0.8);
}

void EffectsManager::DeInit() {
    for (Texture2D& tex : hayeffect) {
        UnloadTexture(tex);
    }
    for (Texture2D& tex : slasheffect) {
        UnloadTexture(tex);
    }
    for (Effect* effect : currentEffects) {
        delete effect;
    }
}

void EffectsManager::SpawnPBPEffect(Vector2 spawnPos) {
    ChunkManager* chunkmanagerptr = (ChunkManager*)ReferencePool::Get("CHUNKMANAGER REF");

    if (!chunkmanagerptr) return;

    // Now that PhysicsBasedParticle inherits from Effect, this will work!
    currentEffects.push_back(new PhysicsBasedParticle(
        &hayeffect,
        HAY_EFFECT_DEFAULT_PARTICLE_COUNT,
        spawnPos,
        HAY_EFFECT_DEFAULT_DIRECTION,
        HAY_EFFECT_DEFAULT_SHAPE,
        HAY_EFFECT_DEFAULT_MIN_SPEED,
        HAY_EFFECT_DEFAULT_MAX_SPEED,
        HAY_EFFECT_DEFAULT_MIN_LIFETIME,
        HAY_EFFECT_DEFAULT_MAX_LIFETIME,
        HAY_EFFECT_DEFAULT_MIN_ROT_SPEED,
        HAY_EFFECT_DEFAULT_MAX_ROT_SPEED,
        HAY_EFFECT_DEFAULT_SCALE,
        HAY_EFFECT_DEFAULT_GRAVITY,
        HAY_EFFECT_DEFAULT_DRAG,
        chunkmanagerptr
    ));
}

void EffectsManager::SpawnPBPEffect(const std::vector<Texture2D> *frames, int particleCount, Vector2 origin, Vector2 direction, ParticleShape shape, float minSpeed, float maxSpeed, float minLifetime, float maxLifetime, float minRotSpeed, float maxRotSpeed, float scale, float gravity, float drag, ChunkManager *chunk) {

    // Explicitly using 'new' to store in the Effect* vector
    currentEffects.push_back(new PhysicsBasedParticle(
        frames, particleCount, origin, direction, shape,
        minSpeed, maxSpeed, minLifetime, maxLifetime,
        minRotSpeed, maxRotSpeed, scale, gravity, drag, chunk
    ));
}

void EffectsManager::SpawnSlashEffect(Vector2 spawnPos) {
    // Uses the DEFAULT defines from your header
    currentEffects.push_back(new SlashEffect(
        &slasheffect,
        spawnPos,
        SLASH_EFFECT_DEFAULT_SPEED,
        SLASH_EFFECT_DEFAULT_SCALE,
        SLASH_EFFECT_DEFAULT_DURATION,
        SLASH_EFFECT_DEFAULT_TYPE,
        SLASH_EFFECT_DEFAULT_COUNT
    ));
}

void EffectsManager::SpawnSlashEffect(const std::vector<Texture2D> *frames, Vector2 origin, float speed, float scale, float duration, SlashType type, int count) {

    currentEffects.push_back(new SlashEffect(
        frames, origin, speed, scale, duration, type, count
    ));
}

void EffectsManager::Draw() {
    for (Effect* effect : currentEffects) {
        effect->Draw();
    }
    std::cout << "Drawing " << currentEffects.size() << " effects" << std::endl;
}

void EffectsManager::Update() {
    double* dt_ptr = (double*)ReferencePool::Get("DELTATIME REF");
    if (!dt_ptr) return;
    double dt = *dt_ptr;

    // 1. Update all effects
    for (Effect* effect : currentEffects) {
        effect->Update(dt);
    }

    // 2. Erase-Remove Idiom with Pointer Cleanup
    // We must delete the object before removing the pointer from the vector
    currentEffects.erase(
        std::remove_if(currentEffects.begin(), currentEffects.end(),
                       [](Effect* e) {
                           if (e->IsFinished()) {
                               delete e; // Clean up heap memory
                               return true;
                           }
                           return false;
                       }),
        currentEffects.end()
    );

    // 3. Maintenance (The "Shrink to Fit" style logic)
    if (GetRandomValue(0, 100) == 5) {
        currentEffects.shrink_to_fit();
    }
}