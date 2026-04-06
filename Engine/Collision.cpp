//
// Created by Manomay Tyagi on 4/6/26.
//

#include "Collision.h"
vector<HasCollider*> Collision::AllColliders;


void Collision::AddCollider(HasCollider *collider) {
    AllColliders.push_back(collider);
}

void Collision::RemoveCollider(HasCollider *collider) {
    AllColliders.erase(
    std::remove(AllColliders.begin(), AllColliders.end(), collider),
    AllColliders.end());
}

void Collision::CheckCollision(HasCollider* collider) {
    if (!collider) return;

    for (auto* other : AllColliders) {
        if (!other || other == collider) continue;

        // Compare all hitboxes of collider vs all hitboxes of other
        for (auto& hbA : collider->hitboxes) {
            for (auto& hbB : other->hitboxes) {
                if (CheckCollisionRecs(hbA.rect, hbB.rect)) {
                    std::cout << "Collision: "
                              << collider->tag << " (" << hbA.name << ")"
                              << " hit "
                              << other->tag << " (" << hbB.name << ")"
                              << std::endl;
                }
            }
        }
    }
}


void Collision::CheckCollision(HasCollider* a, HasCollider* b) {
    if (!a || !b || a == b) return;

    for (auto& hbA : a->hitboxes) {
        for (auto& hbB : b->hitboxes) {
            if (CheckCollisionRecs(hbA.rect, hbB.rect)) {
                std::cout << "Collision: "
                          << a->tag << " (" << hbA.name << ")"
                          << " hit "
                          << b->tag << " (" << hbB.name << ")"
                          << std::endl;
            }
        }
    }
}


void Collision::CheckCollision(Hitbox* hitbox) {
    if (!hitbox) return;

    for (auto* other : AllColliders) {
        if (!other) continue;

        for (auto& hb : other->hitboxes) {
            if (CheckCollisionRecs(hitbox->rect, hb.rect)) {
                std::cout << "Hitbox-only collision: "
                          << hitbox->name
                          << " hit "
                          << other->tag << " (" << hb.name << ")"
                          << std::endl;
            }
        }
    }
}
