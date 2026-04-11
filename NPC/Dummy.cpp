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

    hayFrames = SpriteSplitter::SplitByHorizontal("../Art/Particles/Hay.png", 3, 0.8);
    slashFrames = SpriteSplitter::SplitByHorizontal("../Art/Particles/WhiteSlash.png", 3, 0.8);

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

    // 3. Unload Particle Textures (These were leaked!)
    for (Texture2D& tex : hayFrames) {
        UnloadTexture(tex);
    }
    for (Texture2D& tex : slashFrames) {
        UnloadTexture(tex);
    }

    // 4. Global System Cleanup
    Collision::RemoveCollider(this);
    entitylayer.RemoveDrawCall(this);
    playerposmanager.RemoveListener("DUMMY");

    std::cout << "[DUMMY] Cleaned up resources." << std::endl;
}

void Dummy::Draw() {
    // Draw particle effects
    for (auto& p : hayeffects)
        p.Draw();
    for (auto& s : slasheffects)
        s.Draw();


    Texture2D* tex = animator.GetTexture();
    DrawTexture(*tex, pos.x, pos.y, WHITE);
}


void Dummy::Update(double& dt) {
    animator.ChangeAnimationState("IDLE");
    // Update all particle effects
    for (auto& p : hayeffects)
        p.Update((float)dt);

    for (auto& s : slasheffects)
        s.Update(dt);


    // Auto-remove finished effects
    hayeffects.erase(
        std::remove_if(hayeffects.begin(), hayeffects.end(),
                       [](auto& p){ return p.IsFinished(); }),
        hayeffects.end()
    );

    slasheffects.erase(
    std::remove_if(slasheffects.begin(), slasheffects.end(),
                   [](auto& s){ return s.IsFinished(); }),
    slasheffects.end()

    );
    if (GetRandomValue(0, 100) == 5) {
        hayeffects.shrink_to_fit();
        slasheffects.shrink_to_fit();
    }

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
                40.0f, 40.0f,
                1.5f, 2.0f,
                -6.0f, 6.0f,
                1.0f,
                30.0f,
                0.98f,
                &chunkmanager
                );
                slasheffects.emplace_back(
                &slashFrames,
                Vector2{pos.x + allframes[0].width/2, pos.y+ allframes[0].height/2},
                60.0f,      // speed
                1.0f,        // scale
                0.1f,       // duration
                SLASH_UNIFORM, 10
                );
                GameCamera::PulseZoom(1.1f, 1.0f);
                GameCamera::TriggerShake(3.0f, 1.0f);

                animator.ChangeAnimationState("HURT");
                PlaySound(hit);


            }
        }
    }
}
