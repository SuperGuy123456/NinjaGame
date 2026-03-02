//
// Created by tyagi on 3/1/2026.
//

#ifndef NINJAGAME_PLAYER_H
#define NINJAGAME_PLAYER_H

#include "Engine/DrawingPipeline.h"
#include "Engine/InputManager.h"
#include "Engine/EventManager.h"
#include "BaseClasses/Base.h"

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
    DrawLayer& entitylayer;
    Vector2 pos;
};


#endif //NINJAGAME_PLAYER_H