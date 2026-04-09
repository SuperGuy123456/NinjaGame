#include "Animation.h"
#include <iostream>

Animator::Animator(std::vector<Texture2D>* frameSource)
    : frames(frameSource) {}

void Animator::AddAnimationState(const std::string& name, const Animation& anim) {
    animations[name] = anim;   // safe copy
}

void Animator::ChangeAnimationState(const std::string& name) {
    if (currentName == name)
        return;

    if (blocking)
        return;

    auto& anim = animations.at(name);

    currentName = name;
    active = &anim;

    currentFrame = anim.reverse ? anim.frames.size() - 1 : 0;
    timer = 0.0f;
    loopsRemaining = anim.loopCount;

    blocking = anim.blockStates;

    currentTexture = &(*frames)[active->frames[currentFrame]];
}

void Animator::Animate(float dt) {
    if (!active)
        return;

    timer += dt;

    int count = active->frames.size();

    if (timer >= active->timePerFrame) {
        timer -= active->timePerFrame;

        // Advance frame
        if (active->reverse)
            currentFrame--;
        else
            currentFrame++;

        // Looping logic
        if (active->loop) {
            if (currentFrame >= count) {
                currentFrame = 0;

                if (active->loopCount > 0) {
                    loopsRemaining--;
                    if (loopsRemaining <= 0)
                        currentFrame = count - 1;
                }
            }
            if (currentFrame < 0) {
                currentFrame = count - 1;

                if (active->loopCount > 0) {
                    loopsRemaining--;
                    if (loopsRemaining <= 0)
                        currentFrame = 0;
                }
            }
        }
        else {
            // Clamp for non-looping
            if (currentFrame >= count)
                currentFrame = count - 1;
            if (currentFrame < 0)
                currentFrame = 0;
        }
    }

    // Unblock when animation reaches its final frame
    if (!active->loop) {
        bool lastFrame = active->reverse
            ? (currentFrame == 0)
            : (currentFrame == count - 1);

        if (lastFrame)
            blocking = false;
    }

    currentTexture = &(*frames)[active->frames[currentFrame]];
}

Texture2D* Animator::GetTexture() {
    return currentTexture;
}

bool Animator::IsAnimationFinished() const {
    if (!active) return true;
    if (active->loop) return false;

    bool lastFrame = active->reverse
        ? (currentFrame == 0)
        : (currentFrame == active->frames.size() - 1);

    return lastFrame;
}

