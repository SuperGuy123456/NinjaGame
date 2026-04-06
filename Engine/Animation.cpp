//
// Created by Manomay Tyagi on 4/6/26.
//

#include "Animation.h"

#include <iostream>
#include <ostream>

void Animator::AddAnimationState(const string &name, const Animation &animation) {
    animations.emplace(name, animation);
}

void Animator::ChangeAnimationState(const string &name) {

    // If already playing this animation, do nothing
    if (currentAnimation == name)
        return;

    // If a blocking animation is currently active, do not interrupt
    if (blockingActive)
        return;

    auto& anim = animations.at(name);

    currentAnimation = name;
    activeAnim = &anim;

    currentFrame = anim.reverse ? anim.frames.size() - 1 : 0;
    timer = 0.0f;
    loopsRemaining = anim.loopCount;

    // If this animation is a blocking one, activate block
    blockingActive = anim.blockStates;
}




void Animator::Animate(float deltaTime) {
    if (!activeAnim) {
        return;
    }



    // Advance timer
    timer += deltaTime;



    // Frame info
    int frameCount = activeAnim->frames.size();


    // If enough time has passed, move to next frame
    if (timer >= activeAnim->timePerFrame) {


        timer -= activeAnim->timePerFrame;

        // Reverse playback
        if (activeAnim->reverse) {
            currentFrame--;
        } else {
            currentFrame++;
        }

        // Looping / bounds
        if (!activeAnim->loop) {
            if (currentFrame >= frameCount) currentFrame = frameCount - 1;
            if (currentFrame < 0) currentFrame = 0;
        }
        else {
            if (currentFrame >= frameCount) {
                currentFrame = 0;

                if (activeAnim->loopCount > 0) {
                    loopsRemaining--;
                    if (loopsRemaining <= 0) {
                        currentFrame = frameCount - 1;
                    }
                }
            }

            if (currentFrame < 0) {
                currentFrame = frameCount - 1;

                if (activeAnim->loopCount > 0) {
                    loopsRemaining--;
                    if (loopsRemaining <= 0) {
                        currentFrame = 0;
                    }
                }
            }
        }
    }
    else {

    }



    // Only unblock if we actually ADVANCED into the last frame
    if (!activeAnim->loop &&
        ((activeAnim->reverse && currentFrame == 0) ||
         (!activeAnim->reverse && currentFrame == activeAnim->frames.size() - 1)))
    {
        blockingActive = false;
    }




    // Return the current frame texture
    currenttexture = activeAnim->frames[currentFrame];
}

Texture2D* Animator::GetTexture() {
    return currenttexture;
}