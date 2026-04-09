//
// Created by Manomay Tyagi on 4/6/26.
//

#ifndef NINJAGAME_COLLISION_H
#define NINJAGAME_COLLISION_H

#include "BaseClasses/Base.h"
#include "raylib.h"
#include <vector>
#include <iostream>

using CollisionResult = std::pair<HasCollider*, Hitbox*>;


class Collision {
public:
    static vector<HasCollider*> AllColliders;

    static void AddCollider(HasCollider* collider);
    static void RemoveCollider(HasCollider* collider);

    static bool CheckCollision(HasCollider* collider1, HasCollider* collider2);
    static vector<CollisionResult> CheckCollision(HasCollider* collider);
    static vector<CollisionResult> CheckCollision(Hitbox* hitbox);

};


#endif //NINJAGAME_COLLISION_H