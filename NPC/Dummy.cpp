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

    hayFrames = SpriteSplitter::SplitByHorizontal("../Art/Particles/Hay.png", 3, 8);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Test/FightDummy.png", 15, 10);

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
    for (Texture2D& tex : allframes) {
        UnloadTexture(tex);
    }
    Collision::RemoveCollider(this);
}

void Dummy::Draw() {
    // Draw particle effects
    for (auto& p : hayeffects)
        p.Draw();

    Texture2D* tex = animator.GetTexture();
    DrawTexture(*tex, pos.x, pos.y, WHITE);
}


void Dummy::Update(double& dt) {
    animator.ChangeAnimationState("IDLE");
    // Update all particle effects
    for (auto& p : hayeffects)
        p.Update((float)dt);

    // Auto-remove finished effects
    hayeffects.erase(
        std::remove_if(hayeffects.begin(), hayeffects.end(),
                       [](auto& p){ return p.IsFinished(); }),
        hayeffects.end()
    );

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
                // Spawn hay burst
                hayeffects.emplace_back(
                &hayFrames,          // const std::vector<Texture2D>* frames
                20,                  // particleCount
                Vector2{pos.x + allframes[0].width/2, pos.y+ allframes[0].height/2},
                Vector2{1, 0},
                UNIFORM_EXPLOSION,
                250.0f, 250.0f,
                1.5f, 2.0f,
                -60.0f, 60.0f,
                1.0f,
                300.0f,
                0.9888f,
                &chunkmanager
                );
                animator.ChangeAnimationState("HURT");
                PlaySound(hit);


            }
        }
    }
}
