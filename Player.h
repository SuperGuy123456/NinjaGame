#ifndef NINJAGAME_PLAYER_H
#define NINJAGAME_PLAYER_H

#include "Engine/DrawingPipeline.h"
#include "Engine/InputManager.h"
#include "Engine/EventManager.h"
#include "BaseClasses/Base.h"
#include "Engine/Spritesplitter.h"
#include "Engine/Chunks.h"
#include "Engine/Animation.h"
#include "Engine/Collision.h"
#include "screenres.h"

using namespace std;

class Player : public Load, public Listener, public HasCollider
{
public:
    Player(Vector2 pos,
           DrawLayer& _entitylayer,
           EventManager& _keyboardmanager,
           EventManager& _playerposmanager,
           ChunkManager* _chunkmanager);

    ~Player();

    void Draw() override;
    void Update(double& dt);

    void OnEvent(string &command) override;
    void OnSpecialEvent(string& command, vector<string> params) override {};

    Vector2 pos; // FEET POSITION

private:
    bool IsSolid(float wx, float wy);
    void ResolveCollisions(double dt);

    vector<Texture2D> allframes;

    // Physics
    Vector2 velocity = {0, 0};
    float gravity = 2000.0f;
    float maxFallSpeed = 2000.0f;
    float jumpStrength = -1000.0f;

    // Hitbox (aligned to tiles)
    float width = 20;
    float height = 40;

    // Movement state
    int facing = 1;
    int xchange = 0;
    bool isrunning = false;
    bool grounded = true;
    bool justjumped = false;

    // Ready stance
    bool ready = false;
    bool wasready = false;

    // NEW: Animator
    Animator animator;

    // References
    DrawLayer& entitylayer;
    EventManager& keyboardmanager;
    EventManager& playerposmanager;
    ChunkManager* chunkmanager;
};

#endif
