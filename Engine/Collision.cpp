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

vector<CollisionResult> Collision::CheckCollision(HasCollider* collider) {
    vector<CollisionResult> results;
    if (!collider) return results;

    for (auto* other : AllColliders) {
        if (!other || other == collider) continue;

        for (auto& hbA : collider->hitboxes) {
            for (auto& hbB : other->hitboxes) {
                if (CheckCollisionRecs(hbA.rect, hbB.rect)) {
                    results.push_back({ other, &hbB });
                }
            }
        }
    }

    return results;
}


bool Collision::CheckCollision(HasCollider* a, HasCollider* b) {
    if (!a || !b || a == b) return false;

    for (auto& hbA : a->hitboxes) {
        for (auto& hbB : b->hitboxes) {
            if (CheckCollisionRecs(hbA.rect, hbB.rect)) {
                return true;
            }
        }
    }
    return false;
}


vector<CollisionResult> Collision::CheckCollision(Hitbox* hitbox) {
    vector<CollisionResult> results;
    if (!hitbox) return results;

    for (auto* other : AllColliders) {
        if (!other) continue;

        for (auto& hb : other->hitboxes) {
            if (CheckCollisionRecs(hitbox->rect, hb.rect)) {
                results.push_back({ other, &hb });
            }
        }
    }

    return results;
}
