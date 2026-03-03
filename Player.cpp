//
// Created by tyagi on 3/1/2026.
//

#include "Player.h"

Player::Player(Vector2 pos, DrawLayer& _entitylayer) : entitylayer(_entitylayer){
    this->pos = pos;

    entitylayer.AddDrawCall(this, 0);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 10);
    TraceLog(LOG_INFO, "Player: Loaded %d frames", allframes.size());

    allsequences.push_back(&idle);
    allsequences.push_back(&walk);
    allsequences.push_back( &run);
    allsequences.push_back(&idle_ready);
    allsequences.push_back(&attack1);
    allsequences.push_back(&attack2);

    int fullindex = 0;

    for (int i = 0; i < 6; ++i) {
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
}

void Player::OnEvent(string &command) {

}

void Player::OnSpecialEvent(string &command, vector<string> params) {

}

void Player::Animate()
{
    if (GetTime() - lasttime >= 0.2f)
    {
        lasttime = GetTime();

        // advance frame
        cindex++;

        // wrap around based on animation size
        if (cindex >= sizes[canim])
        {
            cindex = 0;
            //temporarily update the canim
            if (canim < 5) {
                canim++;
            }
            else {
                canim = 0;
            }
        }
    }
}

