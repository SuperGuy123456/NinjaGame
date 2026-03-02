//
// Created by tyagi on 3/1/2026.
//

#include "Player.h"

Player::Player(Vector2 pos, DrawLayer& _entitylayer) : entitylayer(_entitylayer){
    this->pos = pos;

    entitylayer.AddDrawCall(this, 0);
}

Player::~Player() {

}

void Player::Draw() {

}

void Player::Update() {

}

void Player::OnEvent(string &command) {

}

void Player::OnSpecialEvent(string &command, vector<string> params) {

}


