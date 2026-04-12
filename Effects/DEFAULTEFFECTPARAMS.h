//
// Created by Manomay Tyagi on 4/11/26.
//

#ifndef NINJAGAME_DEFAULTEFFECTPARAMS_H
#define NINJAGAME_DEFAULTEFFECTPARAMS_H

#include "Effects/PhysicsBasedParticle.h"
#include "Effects/SlashEffect.h"
#include "raylib.h"

//------------HAY EFFECT---------------

// Texture & Count
#define HAY_EFFECT_DEFAULT_PARTICLE_COUNT 20

// Spawning & Shape
#define HAY_EFFECT_DEFAULT_DIRECTION      Vector2{1, 0}
#define HAY_EFFECT_DEFAULT_SHAPE          UNIFORM_EXPLOSION

// Speed (Your width/height range values)
#define HAY_EFFECT_DEFAULT_MIN_SPEED      40.0f
#define HAY_EFFECT_DEFAULT_MAX_SPEED      40.0f

// Timing/Lifecycle
#define HAY_EFFECT_DEFAULT_MIN_LIFETIME   1.5f
#define HAY_EFFECT_DEFAULT_MAX_LIFETIME   2.0f

// Rotation
#define HAY_EFFECT_DEFAULT_MIN_ROT_SPEED -6.0f
#define HAY_EFFECT_DEFAULT_MAX_ROT_SPEED  6.0f

// Physics & Scale
#define HAY_EFFECT_DEFAULT_SCALE          1.0f
#define HAY_EFFECT_DEFAULT_GRAVITY        30.0f
#define HAY_EFFECT_DEFAULT_DRAG           0.98f

//--------------SLASH EFFECT---------------

// Slash Visuals & Timing
#define SLASH_EFFECT_DEFAULT_SPEED      60.0f
#define SLASH_EFFECT_DEFAULT_SCALE      1.0f
#define SLASH_EFFECT_DEFAULT_DURATION   0.1f

// Slash Logic
#define SLASH_EFFECT_DEFAULT_TYPE       SLASH_UNIFORM
#define SLASH_EFFECT_DEFAULT_COUNT      10

// Camera Feedback (Based on your Pulse/Shake calls)
#define IMPACT_ZOOM_INTENSITY           1.1f
#define IMPACT_ZOOM_SPEED               1.0f
#define IMPACT_SHAKE_INTENSITY          3.0f
#define IMPACT_SHAKE_DURATION           1.0f

#endif //NINJAGAME_DEFAULTEFFECTPARAMS_H