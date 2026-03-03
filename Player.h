//
// Created by tyagi on 3/1/2026.
//

#ifndef NINJAGAME_PLAYER_H
#define NINJAGAME_PLAYER_H

#include "Engine/DrawingPipeline.h"
#include "Engine/InputManager.h"
#include "Engine/EventManager.h"
#include "BaseClasses/Base.h"
#include "Engine/Spritesplitter.h"

using namespace std;

enum AnimState {
    IDLE = 0,
    READYIDLE = 1,
    IDLE_READY = 2,     // ← moved here
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
    Player(Vector2 pos, DrawLayer& _entitylayer);
    ~Player();

    void Draw() override;
    void Update();

    void OnEvent(string &command) override;
    void OnSpecialEvent(string &command, vector<string> params) override;

private:
    void Animate();

    DrawLayer& entitylayer;
    Vector2 pos;

    int canim = 0; //this means idle btw
    int cindex = 0;

    double lasttime = GetTime();

    //magic nums for animations
    const int NUMBEROFSTATES = 15;

    const int IDLESIZE = 4;
    const int READYIDLESIZE = 2;
    const int WALKSIZE = 6;
    const int READYWALKSIZE = 6;
    const int RUNSIZE = 6;
    const int READYRUNSIZE = 6;
    const int IDLE_READYSIZE = 6;
    const int ATTACK1SIZE = 3;
    const int ATTACK2SIZE = 4;
    const int UPJUMPSIZE = 1;
    const int FALLINGSIZE = 1;
    const int READYUPJUMPSIZE = 1;
    const int READYFALLINGSIZE = 1;
    const int LAND_IDLESIZE = 2;
    const int LAND_READYSIZE = 2;

    const int sizes[15] = {
        IDLESIZE,
        READYIDLESIZE,
        IDLE_READYSIZE,   // moved here
        WALKSIZE,
        READYWALKSIZE,
        RUNSIZE,
        READYRUNSIZE,
        ATTACK1SIZE,
        ATTACK2SIZE,
        UPJUMPSIZE,
        FALLINGSIZE,
        READYUPJUMPSIZE,
        READYFALLINGSIZE,
        LAND_IDLESIZE,
        LAND_READYSIZE
    };


    vector<Texture2D> allframes;

    vector<Texture2D*> idle;             // 0
    vector<Texture2D*> readyidle;        // 1
    vector<Texture2D*> walk;             // 2
    vector<Texture2D*> readywalk;        // 3
    vector<Texture2D*> run;              // 4
    vector<Texture2D*> readyrun;         // 5
    vector<Texture2D*> idle_ready;       // 6
    vector<Texture2D*> attack1;          // 7
    vector<Texture2D*> attack2;          // 8
    vector<Texture2D*> upjump;           // 9
    vector<Texture2D*> falling;          // 10
    vector<Texture2D*> readyupjump;      // 11
    vector<Texture2D*> readyfalling;     // 12
    vector<Texture2D*> land_idle;        // 13
    vector<Texture2D*> land_ready;       // 14


    vector<vector<Texture2D*>*> allsequences;
};


#endif //NINJAGAME_PLAYER_H