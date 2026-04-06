#include "Player.h"

Player::Player(Vector2 pos,
               DrawLayer& _entitylayer,
               EventManager& _keyboardmanager,
               EventManager& _playerposmanager,
               ChunkManager* _chunkmanager)
    : entitylayer(_entitylayer),
      keyboardmanager(_keyboardmanager),
      playerposmanager(_playerposmanager),
      chunkmanager(_chunkmanager)
{
    this->pos = pos;
    entitylayer.AddDrawCall(this, 0);

    // -----------------------------
    // LOAD ALL FRAMES
    // -----------------------------
    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 10);

    vector<Texture2D*> ptrs;
    ptrs.reserve(allframes.size());
    for (auto& f : allframes) ptrs.push_back(&f);

    auto Slice = [&](int start, int count) {
        vector<Texture2D*> out;
        out.reserve(count);
        for (int i = 0; i < count; i++)
            out.push_back(ptrs[start + i]);
        return out;
    };

    int index = 0;

    // -----------------------------
    // ANIMATION DEFINITIONS
    // -----------------------------
    Animation idle;
    idle.frames = Slice(index, 4);
    idle.timePerFrame = 0.2f;
    idle.loop = true;
    idle.loopCount = -1;
    animator.AddAnimationState("IDLE", idle);
    index += 4;

    Animation readyidle;
    readyidle.frames = Slice(index, 2);
    readyidle.timePerFrame = 0.2f;
    readyidle.loop = true;
    animator.AddAnimationState("READYIDLE", readyidle);
    index += 2;

    Animation idle_ready;
    idle_ready.frames = Slice(index, 6);
    idle_ready.timePerFrame = 0.15f;
    idle_ready.loop = false;
    idle_ready.blockStates = true;
    animator.AddAnimationState("IDLE_READY", idle_ready);
    index += 6;

    Animation ready_idle = idle_ready;
    ready_idle.reverse = true;
    animator.AddAnimationState("READY_IDLE", ready_idle);

    Animation walk;
    walk.frames = Slice(index, 6);
    walk.timePerFrame = 0.15f;
    walk.loop = true;
    animator.AddAnimationState("WALK", walk);
    index += 6;

    Animation readywalk = walk;
    readywalk.frames = Slice(index, 6);
    animator.AddAnimationState("READYWALK", readywalk);
    index += 6;

    Animation run;
    run.frames = Slice(index, 6);
    run.timePerFrame = 0.1f;
    run.loop = true;
    animator.AddAnimationState("RUN", run);
    index += 6;

    Animation readyrun = run;
    readyrun.frames = Slice(index, 6);
    animator.AddAnimationState("READYRUN", readyrun);
    index += 6;

    Animation attack1;
    attack1.frames = Slice(index, 3);
    attack1.timePerFrame = 0.12f;
    attack1.loop = false;
    attack1.blockStates = true;
    animator.AddAnimationState("ATTACK1", attack1);
    index += 3;

    Animation attack2;
    attack2.frames = Slice(index, 4);
    attack2.timePerFrame = 0.12f;
    attack2.loop = false;
    attack2.blockStates = true;
    animator.AddAnimationState("ATTACK2", attack2);
    index += 4;

    Animation upjump;
    upjump.frames = Slice(index++, 1);
    upjump.timePerFrame = 0.2f;
    animator.AddAnimationState("UPJUMP", upjump);

    Animation falling;
    falling.frames = Slice(index++, 1);
    falling.timePerFrame = 0.2f;
    animator.AddAnimationState("FALLING", falling);

    Animation readyupjump = upjump;
    readyupjump.frames = Slice(index++, 1);
    animator.AddAnimationState("READYUPJUMP", readyupjump);

    Animation readyfalling = falling;
    readyfalling.frames = Slice(index++, 1);
    animator.AddAnimationState("READYFALLING", readyfalling);

    Animation land_idle;
    land_idle.frames = Slice(index, 2);
    land_idle.timePerFrame = 0.15f;
    land_idle.blockStates = true;
    animator.AddAnimationState("LAND_IDLE", land_idle);
    index += 2;

    Animation land_ready = land_idle;
    land_ready.frames = Slice(index, 2);
    animator.AddAnimationState("LAND_READY", land_ready);
    index += 2;

    animator.ChangeAnimationState("IDLE");

    keyboardmanager.AddListener("PLAYER_W", this, "HOLD_W");
    keyboardmanager.AddListener("PLAYER_A", this, "HOLD_A");
    keyboardmanager.AddListener("PLAYER_S", this, "HOLD_S");
    keyboardmanager.AddListener("PLAYER_D", this, "HOLD_D");
    keyboardmanager.AddListener("PLAYER_SHIFT", this, "HOLD_SHIFT");
    keyboardmanager.AddListener("PLAYER_E", this, "PRESS_E");

    keyboardmanager.AddListener("PLAYER_R_W", this, "RELEASE_W");
    keyboardmanager.AddListener("PLAYER_R_A", this, "RELEASE_A");
    keyboardmanager.AddListener("PLAYER_R_S", this, "RELEASE_S");
    keyboardmanager.AddListener("PLAYER_R_D", this, "RELEASE_D");
    keyboardmanager.AddListener("PLAYER_R_SHIFT", this, "RELEASE_SHIFT");

    Rectangle body = { pos.x - width / 2.0f, pos.y - height, width, height };
    this->tag = "PLAYER";
    this->AddHitbox(body, "LEGS", RIDGIDBOX);
    Collision::AddCollider(this);
}

Player::~Player() {
    Collision::RemoveCollider(this);
    for (Texture2D& tex : allframes) UnloadTexture(tex);
}

bool Player::IsSolid(float wx, float wy) {
    return chunkmanager->CheckTileCollision({wx, wy}) != 0;
}

void Player::Update(double& dt) {
    bool wasGrounded = grounded;

    // 1. INPUT
    xchange = 0;
    if (IsKeyDown(KEY_A)) { xchange = -1; facing = -1; }
    if (IsKeyDown(KEY_D)) { xchange = 1; facing = 1; }

    float currentSpeed = (isrunning ? 1000.0f : 600.0f);
    velocity.x = xchange * currentSpeed;

    // 2. GRAVITY (Only apply if not grounded)
    if (!grounded) {
        velocity.y += gravity * dt;
        if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;
    } else {
        // Small constant force to keep grounded check stable
        velocity.y = 10.0f;
    }

    // 3. PHYSICS
    ResolveCollisions(dt);

    // 4. ANIMATION LOGIC
    // We check animator.GetCurrentState() inside the logic if needed,
    // but focusing on fixing the state transitions here.
    if (!grounded) {
        if (velocity.y < 0)
            animator.ChangeAnimationState(ready ? "READYUPJUMP" : "UPJUMP");
        else
            animator.ChangeAnimationState(ready ? "READYFALLING" : "FALLING");
    }
    else {
        if (!wasGrounded) {
            animator.ChangeAnimationState(ready ? "LAND_READY" : "LAND_IDLE");
        }
        else if (xchange != 0) {
            if (isrunning)
                animator.ChangeAnimationState(ready ? "READYRUN" : "RUN");
            else
                animator.ChangeAnimationState(ready ? "READYWALK" : "WALK");
        }
        else {
            if (ready && !wasready)
                animator.ChangeAnimationState("IDLE_READY");
            else if (!ready && wasready)
                animator.ChangeAnimationState("READY_IDLE");
            else
                animator.ChangeAnimationState(ready ? "READYIDLE" : "IDLE");
        }
    }

    wasready = ready;

    animator.Animate(dt);
    playerposmanager.BroadcastSpecialMessage(
        "PLAYER_POS_UPDATE " + to_string(pos.x) + " " + to_string(pos.y)
    );
}

void Player::ResolveCollisions(double dt) {
    // --- HORIZONTAL ---
    pos.x += velocity.x * dt;
    hitboxes[0].rect.x = pos.x - width / 2.0f;
    hitboxes[0].rect.y = pos.y - height;

    for (HasCollider* other : Collision::AllColliders) {
        if (other == this) continue;
        for (Hitbox& hbB : other->hitboxes) {
            if (hbB.type != RIDGIDBOX) continue;
            if (CheckCollisionRecs(hitboxes[0].rect, hbB.rect)) {
                Rectangle overlap = GetCollisionRec(hitboxes[0].rect, hbB.rect);
                if (hitboxes[0].rect.x < hbB.rect.x) pos.x -= overlap.width;
                else pos.x += overlap.width;
                velocity.x = 0;
                hitboxes[0].rect.x = pos.x - width / 2.0f;
            }
        }
    }

    // --- VERTICAL ---
    grounded = false;
    pos.y += velocity.y * dt;
    hitboxes[0].rect.y = pos.y - height;

    for (HasCollider* other : Collision::AllColliders) {
        if (other == this) continue;
        for (Hitbox& hbB : other->hitboxes) {
            if (hbB.type != RIDGIDBOX) continue;
            if (CheckCollisionRecs(hitboxes[0].rect, hbB.rect)) {
                Rectangle overlap = GetCollisionRec(hitboxes[0].rect, hbB.rect);

                // If moving down or steady, and overlap is mostly from top
                if (velocity.y >= 0 && (hitboxes[0].rect.y + hitboxes[0].rect.height) <= (hbB.rect.y + overlap.height + 1.0f)) {
                    pos.y -= overlap.height;
                    grounded = true;
                    velocity.y = 0;
                }
                else if (velocity.y < 0) { // Hitting ceiling
                    pos.y += overlap.height;
                    velocity.y = 0;
                }
                hitboxes[0].rect.y = pos.y - height;
            }
        }
    }
}

void Player::Draw() {
    Texture* tex = animator.GetTexture();
    Rectangle src = {0, 0, facing * (float)tex->width, (float)tex->height};
    Rectangle dest = {pos.x, pos.y+1, (float)tex->width, (float)tex->height};
    Vector2 origin = {tex->width/2.0f, (float)tex->height};

    DrawTexturePro(*tex, src, dest, origin, 0, WHITE);

    if (DEBUG_COLLISION) {
        DrawRectangleLinesEx(hitboxes[0].rect, 1, RED);
    }
}

void Player::OnEvent(string &command) {
    if (command == "HOLD_W") {
        if (grounded) {
            velocity.y = jumpStrength;
            grounded = false;
        }
    }
    if (command == "HOLD_A") xchange = -1;
    if (command == "HOLD_D") xchange = 1;
    if (command == "HOLD_SHIFT") isrunning = true;
    if (command == "PRESS_E") ready = !ready;
    if (command == "RELEASE_A" || command == "RELEASE_D") xchange = 0;
    if (command == "RELEASE_SHIFT") isrunning = false;
}