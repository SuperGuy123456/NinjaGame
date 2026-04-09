#pragma once
#include <string>
#include <map>
#include <vector>
#include "raylib.h"
#include <iostream>

using namespace std;

struct Animation {
    std::vector<int> frames;   // indices into the texture array
    float timePerFrame = 0.1f;

    bool loop = false;
    int loopCount = -1;        // -1 = infinite

    bool reverse = false;
    bool blockStates = false;
};

class Animator {
public:
    Animator(std::vector<Texture2D>* frameSource);

    void AddAnimationState(const std::string& name, const Animation& anim);
    void ChangeAnimationState(const std::string& name);

    void Animate(float dt);
    Texture2D* GetTexture();

    bool IsAnimationFinished() const;

private:
    std::map<std::string, Animation> animations;

    std::string currentName = "";
    Animation* active = nullptr;

    std::vector<Texture2D>* frames;   // pointer to real texture storage

    int currentFrame = 0;
    float timer = 0.0f;
    int loopsRemaining = -1;

    bool blocking = false;

    Texture2D* currentTexture = nullptr;
};
