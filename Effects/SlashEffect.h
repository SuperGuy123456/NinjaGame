#pragma once

#include "raylib.h"
#include <vector>
#include "BaseClasses/Base.h"

enum SlashType {
    SLASH_ONE_SIDE,
    SLASH_DOUBLE_SIDE,
    SLASH_UNIFORM
};

class SlashEffect : public Effect{
public:
    SlashEffect(
    const std::vector<Texture2D>* frames,
    Vector2 origin,
    float speed,
    float scale,
    float duration,
    SlashType type,
    int count        // <--- NEW
);


    void Update(double dt) override;
    void Draw() override;
    bool IsFinished() const override{ return dead; }

private:
    const std::vector<Texture2D>* textures;

    struct SlashParticle {
        Vector2 pos;
        Vector2 vel;
        float rotation;
        float age;
        float duration;
        float scale;
        float squish;     // 1.0 → 0.0 over time
        int frameIndex;
    };

    std::vector<SlashParticle> particles;
    bool dead = false;
};
