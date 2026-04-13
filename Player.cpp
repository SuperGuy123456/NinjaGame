#include "Player.h"

Player::Player(Vector2 pos,
               DrawLayer& _entitylayer,
               EventManager& _keyboardmanager,
               EventManager& _playerposmanager,
               ChunkManager* _chunkmanager)
    : entitylayer(_entitylayer),
      keyboardmanager(_keyboardmanager),
      playerposmanager(_playerposmanager),
      chunkmanager(_chunkmanager),
      animator(&allframes)   // ✅ IMPORTANT: hook animator to frame storage
{
    this->pos = pos;
    entitylayer.AddDrawCall(this, 10000);

    // -----------------------------
    // LOAD ALL FRAMES
    // -----------------------------
    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 1);

    // Helper: return a vector of frame indices instead of pointers
    auto Slice = [&](int start, int count) {
        std::vector<int> out;
        out.reserve(count);
        for (int i = 0; i < count; i++)
            out.push_back(start + i);
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
    run.blockStates = false;
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

    // -----------------------------
    // INPUT LISTENERS
    // -----------------------------
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

    keyboardmanager.AddListener("PRESS_LEFT CLICK", this, "PRESS_LEFT CLICK");
    keyboardmanager.AddListener("RELEASE_LEFT CLICK", this, "RELEASE_LEFT_CLICK");

    // -----------------------------
    // COLLISION BOXES
    // -----------------------------
    Rectangle legs = { pos.x - ridgidbox_width / 2.0f, pos.y - ridgidbox_height, ridgidbox_width, ridgidbox_height };
    this->tag = "PLAYER";
    this->AddHitbox(legs, "LEGS", RIDGIDBOX);

    Rectangle attackbox = { pos.x - ridgidbox_width, pos.y - ridgidbox_height, ridgidbox_width, ridgidbox_height };
    this->AddHitbox(attackbox, "PLAYER_ATTACKBOX", ATTACKBOX);

    Collision::AddCollider(this);
}


Player::~Player() {
    // 1. Remove from Global Systems
    Collision::RemoveCollider(this);
    entitylayer.RemoveDrawCall(this); // <-- ADD THIS

    // 2. Remove Event Listeners (Must match constructor IDs exactly)
    keyboardmanager.RemoveListener("PLAYER_W");
    keyboardmanager.RemoveListener("PLAYER_A");
    keyboardmanager.RemoveListener("PLAYER_S");
    keyboardmanager.RemoveListener("PLAYER_D");
    keyboardmanager.RemoveListener("PLAYER_SHIFT");
    keyboardmanager.RemoveListener("PLAYER_E");
    keyboardmanager.RemoveListener("PLAYER_R_W");
    keyboardmanager.RemoveListener("PLAYER_R_A");
    keyboardmanager.RemoveListener("PLAYER_R_S");
    keyboardmanager.RemoveListener("PLAYER_R_D");
    keyboardmanager.RemoveListener("PLAYER_R_SHIFT");

    // Corrected these to match your constructor IDs
    keyboardmanager.RemoveListener("PRESS_LEFT CLICK");
    keyboardmanager.RemoveListener("RELEASE_LEFT CLICK");

    // 3. Free GPU Memory
    for (Texture2D& tex : allframes) {
        UnloadTexture(tex);
    }
}

bool Player::IsSolid(float wx, float wy) {
    return chunkmanager->CheckTileCollision({wx, wy}) != 0;
}

void Player::Update(double& dt) {
    if (GameCamera::usingFreeCam) {
        xchange = 0;
        velocity = {0,0};
        isrunning = false;
        return;
    }

    bool wasGrounded = grounded;

    // ---------------------------------------------------------
    // 1. MOVEMENT INPUT
    // ---------------------------------------------------------
    xchange = 0;
    if (IsKeyDown(KEY_A)) { xchange = -1; facing = -1; }
    if (IsKeyDown(KEY_D)) { xchange = 1; facing = 1; }

    float currentSpeed = (isrunning ? 100.0f : 60.0f); // Was 1000 and 600
    velocity.x = xchange * currentSpeed;

    // ---------------------------------------------------------
    // 2. PHYSICS & GRAVITY
    // ---------------------------------------------------------
    if (!grounded) {
        velocity.y += gravity * dt;
        if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;
    } else {
        velocity.y = 10.0f; // Stick to ground
    }

    ResolveCollisions(dt);

    // ---------------------------------------------------------
    // 3. COMBAT SYSTEM (ATTACK1 → ATTACK2 BUFFERING)
    // ---------------------------------------------------------
    if (isAttacking) {

        if (animator.IsAnimationFinished()) {
            if (currentAttack == 1 && bufferedAttack) {
                std::cout << "[COMBAT] Attack1 ENDED → Attack2 STARTING (buffered)" << std::endl;

                bufferedAttack = false;
                currentAttack = 2;

                // --- THE FIX ---

                animator.ChangeAnimationState("ATTACK2");
                playerposmanager.BroadcastMessage("ATTACKED");
                // ---------------
            }
            else {
                isAttacking = false;
                currentAttack = 0;

            }
        }
    }



    // ---------------------------------------------------------
    // 3. ANIMATION STATE MACHINE (CLEAN)
    // ---------------------------------------------------------
    if (!isAttacking) {
        // Check Airborne States
        if (!grounded) {
            if (velocity.y < 0)
                animator.ChangeAnimationState(ready ? "READYUPJUMP" : "UPJUMP");
            else
                animator.ChangeAnimationState(ready ? "READYFALLING" : "FALLING");
        }
        // Check Grounded States
        else {
            // Just landed
            if (!wasGrounded) {
                animator.ChangeAnimationState(ready ? "LAND_READY" : "LAND_IDLE");
            }
            // Moving
            else if (xchange != 0) {
                if (isrunning)
                    animator.ChangeAnimationState(ready ? "READYRUN" : "RUN");
                else
                    animator.ChangeAnimationState(ready ? "READYWALK" : "WALK");
            }
            // Standing Still
            else {
                // Handle Transitions between Idle and Ready
                if (ready && !wasready)
                    animator.ChangeAnimationState("IDLE_READY");
                else if (!ready && wasready)
                    animator.ChangeAnimationState("READY_IDLE");
                else
                    animator.ChangeAnimationState(ready ? "READYIDLE" : "IDLE");
            }
        }

        wasready = ready;
    }


    // ---------------------------------------------------------
    // 4. WRAP UP
    // ---------------------------------------------------------
    animator.Animate(dt);

    playerposmanager.BroadcastSpecialMessage(
        "PLAYER_POS_UPDATE " + to_string(pos.x) + " " + to_string(pos.y)
    );
}

void Player::ResolveCollisions(double dt) {
    // A tiny epsilon to prevent "micro-stuck" states
    const float skin = 0.01f;

    // --- STEP 1: HORIZONTAL MOVEMENT ---
    pos.x += velocity.x * dt;

    // Update hitboxes for X check
    hitboxes[0].rect.x = pos.x - ridgidbox_width / 2.0f;
    // Attack box sync
    if (facing == 1) hitboxes[1].rect.x = pos.x;
    else hitboxes[1].rect.x = pos.x - ridgidbox_width * 1.5f;

    vector<CollisionResult> xCollisions = Collision::CheckCollision(this);
    for (auto& res : xCollisions) {
        Hitbox* hbB = res.second;
        if (hbB->type != RIDGIDBOX) continue;

        Rectangle overlap = GetCollisionRec(hitboxes[0].rect, hbB->rect);

        // Use center-comparison to decide which way to push
        float playerCenterX = hitboxes[0].rect.x + hitboxes[0].rect.width / 2.0f;
        float blockCenterX = hbB->rect.x + hbB->rect.width / 2.0f;

        if (playerCenterX < blockCenterX) {
            pos.x -= (overlap.width + skin);
        } else {
            pos.x += (overlap.width + skin);
        }

        // Only kill velocity if we are actually moving into the wall
        // This allows physics forces from other sources to keep working
        velocity.x = 0;

        // Sync for subsequent checks
        hitboxes[0].rect.x = pos.x - ridgidbox_width / 2.0f;
        if (facing == 1) hitboxes[1].rect.x = pos.x;
        else hitboxes[1].rect.x = pos.x - ridgidbox_width * 1.5f;
    }

    // --- STEP 2: VERTICAL MOVEMENT ---
    grounded = false;
    pos.y += velocity.y * dt;

    hitboxes[0].rect.y = pos.y - ridgidbox_height;
    hitboxes[1].rect.y = pos.y - ridgidbox_height;

    vector<CollisionResult> yCollisions = Collision::CheckCollision(this);
    for (auto& res : yCollisions) {
        Hitbox* hbB = res.second;
        if (hbB->type != RIDGIDBOX) continue;

        Rectangle overlap = GetCollisionRec(hitboxes[0].rect, hbB->rect);

        // Check if we are landing (falling down)
        if (velocity.y > 0) {
            // Ensure we only snap to top if we were actually above the tile
            if (hitboxes[0].rect.y + hitboxes[0].rect.height <= hbB->rect.y + overlap.height + skin + 1.0f) {
                pos.y -= (overlap.height + skin);
                grounded = true;
                velocity.y = 0;
            }
        }
        // Check if we hit a ceiling (jumping up)
        else if (velocity.y < 0) {
            pos.y += (overlap.height + skin);
            velocity.y = 0;
        }

        // Final Sync
        hitboxes[0].rect.y = pos.y - ridgidbox_height;
        hitboxes[1].rect.y = pos.y - ridgidbox_height;
    }
}

void Player::Draw() {
    Texture* tex = animator.GetTexture();
    Rectangle src = {0, 0, facing * (float)tex->width, (float)tex->height};
    Rectangle dest = {pos.x, pos.y, (float)tex->width, (float)tex->height};
    Vector2 origin = {tex->width/2.0f, (float)tex->height};

    DrawTexturePro(*tex, src, dest, origin, 0, WHITE);

    if (DEBUG_COLLISION) {
        DrawRectangleLinesEx(hitboxes[0].rect, 1, GREEN);
        DrawRectangleLinesEx(hitboxes[1].rect, 1, RED);
    }
}

void Player::OnEvent(string &command) {
    if (GameCamera::usingFreeCam) {
        xchange = 0;
        velocity = {0,0};
        isrunning = false;
        return;
    }

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
    if (command == "PRESS_LEFT CLICK" and ready == true) {

        // If not attacking, start Attack1
        if (!isAttacking) {
            isAttacking = true;
            currentAttack = 1;
            animator.ChangeAnimationState("ATTACK1");

            std::cout << "[COMBAT] Attack1 STARTED" << std::endl;
            playerposmanager.BroadcastMessage("ATTACKED");
        }
        // If already in Attack1, buffer Attack2
        else if (isAttacking && currentAttack == 1) {
            bufferedAttack = true;
            std::cout << "[COMBAT] Attack2 BUFFERED during Attack1" << std::endl;
        }
    }


}