//
// Created by Manomay Tyagi on 4/6/26.
//

#include "Dummy.h"

Dummy::Dummy(Vector2 pos, DrawLayer& _entitylayer, EventManager& _playerposmanager, ChunkManager& _chunkmanager)
    : entitylayer(_entitylayer),
      playerposmanager(_playerposmanager),
      animator(&allframes),
      chunkmanager(_chunkmanager)
{
    hit = LoadSound("../Audio/Fight/HardSmack.wav");
    SetSoundVolume(hit, 2.0f);

    entitylayer.AddDrawCall(this, 0);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Test/FightDummy.png", 15, 1);

    Animation idle;
    idle.frames = {0 };
    idle.timePerFrame = 0.2f;
    idle.loop = true;
    animator.AddAnimationState("IDLE", idle);

    Animation hurt;
    hurt.frames = { 0,1,2,3 };
    hurt.timePerFrame = 0.15;
    hurt.loop = false;
    hurt.blockStates = true;
    animator.AddAnimationState("HURT", hurt);

    animator.ChangeAnimationState("IDLE");

    Rectangle hurtbox = { pos.x, pos.y, (float)allframes[0].width, (float)allframes[0].height };
    tag = "DUMMY";
    AddHitbox(hurtbox, "HURTBOX", HURTBOX);

    Collision::AddCollider(this);

    this->pos = pos;

    playerposmanager.AddListener("DUMMY", this, "ATTACKED");

    std::cout << "[DUMMY] Spawned at: " << pos.x << ", " << pos.y << std::endl;
}


Dummy::~Dummy() {
    // 1. Unload Sound
    UnloadSound(hit);

    // 2. Unload Main Textures
    for (Texture2D& tex : allframes) {
        UnloadTexture(tex);
    }

    // 4. Global System Cleanup
    Collision::RemoveCollider(this);
    entitylayer.RemoveDrawCall(this);
    playerposmanager.RemoveListener("DUMMY");

    std::cout << "[DUMMY] Cleaned up resources." << std::endl;
}

void Dummy::Draw() {
    Texture2D* tex = animator.GetTexture();
    DrawTexture(*tex, pos.x, pos.y, WHITE);
}


void Dummy::Update(double& dt) {
    animator.ChangeAnimationState("IDLE");
    animator.Animate((float)dt);
}


void Dummy::OnEvent(string &command) {
    if (command!="ATTACKED"){return;}

    vector<CollisionResult>  objects = Collision::CheckCollision(this);

    for (CollisionResult& obj : objects) {
        if (obj.first->tag == "PLAYER") {
            if (obj.second->type == ATTACKBOX) {
                //this means attack box overlapped with hurtbox when the player attacked
                cout<<"HIT BY PLAYER"<<endl;

                GameCamera::PulseZoom(1.1f, 1.0f);
                GameCamera::TriggerShake(3.0f, 1.0f);


                EffectsManager::SpawnPBPEffect(Vector2{pos.x + allframes[0].width/2, pos.y+ allframes[0].height/2});
                EffectsManager::SpawnSlashEffect(Vector2{pos.x + allframes[0].width/2, pos.y+ allframes[0].height/2});
                animator.ChangeAnimationState("HURT");
                PlaySound(hit);


            }
        }
    }
}
