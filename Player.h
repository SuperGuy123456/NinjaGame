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
    const int IDLESIZE = 4;
    const int WALKSIZE = 6;
    const int RUNSIZE = 6;
    const int IDLE_READYSIZE = 6;
    const int ATTACK1SIZE = 3;
    const int ATTACK2SIZE = 4;

    const int sizes[6] = {IDLESIZE, WALKSIZE, RUNSIZE, IDLE_READYSIZE, ATTACK1SIZE, ATTACK2SIZE};

    vector<Texture2D> allframes;

    vector<Texture2D*> idle;
    vector<Texture2D*> walk;
    vector<Texture2D*> run;
    vector<Texture2D*> idle_ready;
    vector<Texture2D*> attack1;
    vector<Texture2D*> attack2;

    vector<vector<Texture2D*>*> allsequences;
};


#endif //NINJAGAME_PLAYER_H