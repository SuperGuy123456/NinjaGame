//
// Created by Manomay Tyagi on 4/6/26.
//

#ifndef NINJAGAME_DUMMY_H
#define NINJAGAME_DUMMY_H
#include "raylib.h"
#include "Engine/DrawingPipeline.h"
#include "Engine/Collision.h"
#include "Animation.h"
#include "EventManager.h"
#include "BaseClasses/Base.h"
#include "Engine/Spritesplitter.h"
#include "Effects/PhysicsBasedParticle.h"
#include "Effects/SlashEffect.h"
#include "Engine/GameCamera.h"

/*
 * super simple dummy that waits to get hit and tests effects and sound >_<
 */

class Dummy : public Load, HasCollider, Listener{
public:
    Dummy(Vector2 pos, DrawLayer& _entitylayer, EventManager& _playerposmanager, ChunkManager& _chunkmanager);
    ~Dummy();

    void Draw() override;
    void Update(double& dt);

    void OnEvent(string &command) override ;
    void OnSpecialEvent(string &command, vector<string> params) override {};
private:
    DrawLayer& entitylayer;
    EventManager& playerposmanager;
    ChunkManager& chunkmanager;

    vector<Texture2D> allframes;
    Animator animator;

    Vector2 pos;

    Sound hit;

    vector<PhysicsBasedParticle> hayeffects;
    vector<SlashEffect> slasheffects;
    vector<Texture2D> slashFrames;
    vector<Texture2D> hayFrames;
};


#endif //NINJAGAME_DUMMY_H