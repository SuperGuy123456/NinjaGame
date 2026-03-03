//
// Created by tyagi on 3/1/2026.
//

#include "Player.h"

Player::Player(Vector2 pos, DrawLayer& _entitylayer) : entitylayer(_entitylayer){
    this->pos = pos;

    entitylayer.AddDrawCall(this, 0);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 10);
    TraceLog(LOG_INFO, "Player: Loaded %d frames", allframes.size());

    allsequences.push_back(&idle);          // 0
    allsequences.push_back(&readyidle);     // 1
    allsequences.push_back(&idle_ready);    // 2  ← moved here
    allsequences.push_back(&walk);          // 3
    allsequences.push_back(&readywalk);     // 4
    allsequences.push_back(&run);           // 5
    allsequences.push_back(&readyrun);      // 6
    allsequences.push_back(&attack1);       // 7
    allsequences.push_back(&attack2);       // 8
    allsequences.push_back(&upjump);        // 9
    allsequences.push_back(&falling);       // 10
    allsequences.push_back(&readyupjump);   // 11
    allsequences.push_back(&readyfalling);  // 12
    allsequences.push_back(&land_idle);     // 13
    allsequences.push_back(&land_ready);    // 14



    int fullindex = 0;

    for (int i = 0; i < NUMBEROFSTATES; ++i) {
        for (int j = 0; j < sizes[i]; ++j) {
            allsequences[i]->push_back(&allframes[fullindex]);
            fullindex++;
        }
    }

}

Player::~Player() {
    for (const Texture2D& tex : allframes) {
        UnloadTexture(tex);
    }
}

void Player::Draw() {
    DrawTexture(*(*allsequences[canim])[cindex], pos.x, pos.y, WHITE);
}

void Player::Update() {
    Animate();
    canim = LAND_READY;
}

void Player::OnEvent(string &command) {

}

void Player::OnSpecialEvent(string &command, vector<string> params) {

}

void Player::Animate()
{
    // if the animation changed externally (input), reset frame index
    static int lastAnim = -1;
    if (canim != lastAnim) {
        cindex = 0;
        lastAnim = canim;
    }

    // frame timing
    if (GetTime() - lasttime >= 0.2f)
    {
        lasttime = GetTime();
        cindex++;

        // loop animation
        if (cindex >= sizes[canim])
        {
            cindex = 0;
        }
    }
}


