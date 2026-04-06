//
// Created by Manomay Tyagi on 4/6/26.
//

#ifndef NINJAGAME_COLLISION_H
#define NINJAGAME_COLLISION_H

#include "BaseClasses/Base.h"
#include "raylib.h"
#include <vector>
#include <iostream>

class Collision {
public:
    static vector<HasCollider*> AllColliders;

    static void AddCollider(HasCollider* collider);
    static void RemoveCollider(HasCollider* collider);

    static void CheckCollision(HasCollider* collider); //makes sure to ignore itself in the vector
    static void CheckCollision(HasCollider* collider1, HasCollider* collider2);
    static void CheckCollision(Hitbox* hitbox);
};


#endif //NINJAGAME_COLLISION_H