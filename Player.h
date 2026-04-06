#ifndef NINJAGAME_PLAYER_H
#define NINJAGAME_PLAYER_H

#include "Engine/DrawingPipeline.h"
#include "Engine/InputManager.h"
#include "Engine/EventManager.h"
#include "BaseClasses/Base.h"
#include "Engine/Spritesplitter.h"
#include "Engine/Chunks.h"

using namespace std;

enum AnimState {
    IDLE = 0,
    READYIDLE = 1,
    IDLE_READY = 2,
    WALK = 3,
    READYWALK = 4,
    RUN = 5,
    READYRUN = 6,
    ATTACK1 = 7,
    ATTACK2 = 8,
    UPJUMP = 9,
    FALLING = 10,
    READYUPJUMP = 11,
    READYFALLING = 12,
    LAND_IDLE = 13,
    LAND_READY = 14
};

class Player : public Load, public Listener
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
    void OnSpecialEvent(string &command, vector<string> params) override;

    Vector2 pos; // FEET POSITION

private:
    void Animate();
    bool IsSolid(float wx, float wy);

    // Physics
    Vector2 velocity = {0, 0};
    float gravity = 2000.0f;
    float maxFallSpeed = 2000.0f;
    float jumpStrength = -700.0f;

    // Hitbox (aligned to tiles)
    float width = 20;
    float height = 40;

    // Movement state
    int facing = 1;
    int xchange = 0;
    bool isrunning = false;
    bool grounded = true;
    bool justjumped = false;

    float animSpeed = 0.2f;       // default speed for normal animations
    float transitionSpeed = 0.35f; // slower transitions

    // Ready stance
    bool ready = false;
    bool wasready = false;
    bool playingTransition = false;
    bool playingReverseTransition = false;

    // Animation
    int canim = 0;
    int cindex = 0;
    double lasttime = GetTime();

    const int NUMBEROFSTATES = 15;

    const int sizes[15] = {
        4, 2, 6, 6, 6, 6, 6,
        3, 4, 1, 1, 1, 1, 2, 2
    };

    vector<Texture2D> allframes;

    vector<Texture2D*> idle;
    vector<Texture2D*> readyidle;
    vector<Texture2D*> idle_ready;
    vector<Texture2D*> walk;
    vector<Texture2D*> readywalk;
    vector<Texture2D*> run;
    vector<Texture2D*> readyrun;
    vector<Texture2D*> attack1;
    vector<Texture2D*> attack2;
    vector<Texture2D*> upjump;
    vector<Texture2D*> falling;
    vector<Texture2D*> readyupjump;
    vector<Texture2D*> readyfalling;
    vector<Texture2D*> land_idle;
    vector<Texture2D*> land_ready;

    vector<vector<Texture2D*>*> allsequences;

    // References
    DrawLayer& entitylayer;
    EventManager& keyboardmanager;
    EventManager& playerposmanager;
    ChunkManager* chunkmanager;
};

#endif
