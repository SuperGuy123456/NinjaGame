//
// Created by Manomay Tyagi on 4/6/26.
//

#ifndef NINJAGAME_ANIMATION_H
#define NINJAGAME_ANIMATION_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Animation {
    vector<Texture2D*> frames;   // All frames for this animation
    float timePerFrame = 0.2;          // Duration of each frame (defualt)
    bool blockStates;            // If true, cannot be interrupted
    bool reverse;                // Play backwards
    bool loop;                   // Should animation loop
    int loopCount;               // -1 = infinite, 0 = no loop, >0 = finite loops
};

class Animator {
public:
    Animator() = default;
    ~Animator() = default;

    void AddAnimationState(const string& name, const Animation& animation);
    void ChangeAnimationState(const string& name);

    void Animate(float deltaTime);

    string GetCurrentAnimation() const { return currentAnimation; }

    Texture2D* GetTexture();

private:
    unordered_map<string, Animation> animations;

    string currentAnimation;
    Animation* activeAnim = nullptr;
    Texture2D *currenttexture;

    int currentFrame = 0;
    float timer = 0.0f;
    int loopsRemaining = -1; // -1 = infinite
    bool blockingActive = false;
};

#endif //NINJAGAME_ANIMATION_H
