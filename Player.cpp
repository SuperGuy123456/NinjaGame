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
    this->pos = pos; // FEET POSITION

    entitylayer.AddDrawCall(this, 0);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 10);

    allsequences = {
        &idle, &readyidle, &idle_ready, &walk, &readywalk,
        &run, &readyrun, &attack1, &attack2, &upjump,
        &falling, &readyupjump, &readyfalling, &land_idle, &land_ready
    };

    int fullindex = 0;
    for (int i = 0; i < NUMBEROFSTATES; ++i)
        for (int j = 0; j < sizes[i]; ++j)
            allsequences[i]->push_back(&allframes[fullindex++]);

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
}

Player::~Player() {
    for (const Texture2D& tex : allframes)
        UnloadTexture(tex);
}

bool Player::IsSolid(float wx, float wy) {
    return chunkmanager->CheckTileCollision({wx, wy}) != 0;
}

void Player::Update(double& dt) {
    Animate();

    // 1. Input Polling
    xchange = 0;
    if (IsKeyDown(KEY_A)) { xchange = -1; facing = -1; }
    if (IsKeyDown(KEY_D)) { xchange = 1; facing = 1; }

    // 2. Gravity
    // Only apply gravity if we aren't already confirmed grounded from the previous frame
    if (!grounded) {
        velocity.y += gravity * dt;
        if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;
    } else {
        velocity.y = 0;
    }

    float currentSpeed = (isrunning ? 600.0f : 400.0f);
    velocity.x = xchange * currentSpeed;

    const float TILE_SIZE = 8.0f;
    const float EPS = 0.1f;

    // 3. MOVE X
    pos.x += velocity.x * dt;
    if (velocity.x > 0) {
        if (IsSolid(pos.x + width/2, pos.y - 2) || IsSolid(pos.x + width/2, pos.y - height + 2)) {
            pos.x = floor((pos.x + width/2) / TILE_SIZE) * TILE_SIZE - width/2;
            velocity.x = 0;
        }
    } else if (velocity.x < 0) {
        if (IsSolid(pos.x - width/2, pos.y - 2) || IsSolid(pos.x - width/2, pos.y - height + 2)) {
            pos.x = ceil((pos.x - width/2) / TILE_SIZE) * TILE_SIZE + width/2;
            velocity.x = 0;
        }
    }

    // 4. MOVE Y
    pos.y += velocity.y * dt;

    // PRE-COLLISION CHECK: Assume we are in air unless floor is hit
    bool wasGrounded = grounded;
    grounded = false;

    if (velocity.y >= 0) { // Falling or Stationary
        // Check a tiny bit below the feet (pos.y + 1)
        if (IsSolid(pos.x - width/2 + EPS, pos.y + EPS) || IsSolid(pos.x + width/2 - EPS, pos.y + EPS)) {
            pos.y = floor(pos.y / TILE_SIZE) * TILE_SIZE;
            velocity.y = 0;
            grounded = true;
        }
    } else if (velocity.y < 0) { // Jumping
        if (IsSolid(pos.x - width/2 + EPS, pos.y - height) || IsSolid(pos.x + width/2 - EPS, pos.y - height)) {
            pos.y = ceil((pos.y - height) / TILE_SIZE) * TILE_SIZE + height;
            velocity.y = 0;
        }
    }

    // 5. Animation State Machine
    // Now grounded is stable for the rest of this frame
    if (!grounded) {
        if (velocity.y < 0) canim = ready ? READYUPJUMP : UPJUMP;
        else               canim = ready ? READYFALLING : FALLING;
    }
    else {
        // Handle Landing
        if (!wasGrounded) {
             canim = ready ? LAND_READY : LAND_IDLE;
        }
        else if (xchange != 0) {
            if (isrunning) canim = ready ? READYRUN : RUN;
            else           canim = ready ? READYWALK : WALK;
        }
        else {
            canim = ready ? READYIDLE : IDLE;
        }
    }

    // 5. Broadcast Position (Send as floats for smooth camera, let the Draw call handle snapping)
    playerposmanager.BroadcastSpecialMessage(
        "PLAYER_POS_UPDATE " + to_string(pos.x) + " " + to_string(pos.y)
    );

    // --- ANIMATION STATE MACHINE ---
    int nextAnim = canim;

    // 1. AIRBORNE
    if (!grounded) {
        if (velocity.y < 0) nextAnim = ready ? READYUPJUMP : UPJUMP;
        else               nextAnim = ready ? READYFALLING : FALLING;
    }
    // 2. GROUNDED
    else {
        // Check if we are currently in a "One-Shot" transition
        bool isTransitioning = (canim == IDLE_READY || canim == LAND_IDLE || canim == LAND_READY);

        // If we are transitioning, wait until the last frame to allow a switch
        // UNLESS the player starts moving (interrupting the transition)
        if (isTransitioning && cindex < sizes[canim] - 1 && xchange == 0 && !justjumped) {
            nextAnim = canim; // Stay in current transition
        }
        else {
            if (xchange != 0) {
                if (isrunning) nextAnim = ready ? READYRUN : RUN;
                else           nextAnim = ready ? READYWALK : WALK;
            }
            else {
                // Handle the switch into Ready mode
                if (ready && !wasready) {
                    nextAnim = IDLE_READY;
                }
                // Handle the switch out of Ready mode (if you have a ready_to_idle anim)
                else if (!ready && wasready) {
                    // If you don't have a specific ready->idle anim, it will snap to IDLE
                    nextAnim = IDLE;
                }
                else {
                    nextAnim = ready ? READYIDLE : IDLE;
                }
            }
        }
    }

    // Final landing check (if we just hit the ground)
    if (grounded && !wasGrounded) {
        nextAnim = ready ? LAND_READY : LAND_IDLE;
    }

    canim = nextAnim;
    wasready = ready;
}

void Player::Draw() {
    if (cindex > sizes[canim] - 1)
        cindex = 0;

    Texture2D* tex = (*allsequences[canim])[cindex];

    Rectangle src = {0, 0, facing * (float)tex->width, (float)tex->height};
    Rectangle dest = {pos.x, pos.y+1, (float)tex->width, (float)tex->height};
    Vector2 origin = {tex->width/2.0f, (float)tex->height};

    DrawTexturePro(*tex, src, dest, origin, 0, WHITE);
}

void Player::OnEvent(string &command) {

    if (command == "HOLD_W") {
        if (grounded) {
            velocity.y = jumpStrength;
            grounded = false;
            justjumped = true;
        }
    }
    if (command == "HOLD_A") xchange = -1;
    if (command == "HOLD_D") xchange = 1;
    if (command == "HOLD_SHIFT") isrunning = true;

    if (command == "PRESS_E") ready = !ready;

    if (command == "RELEASE_A" || command == "RELEASE_D")
        xchange = 0;

    if (command == "RELEASE_SHIFT")
        isrunning = false;
}

void Player::OnSpecialEvent(string &command, vector<string> params) {}

void Player::Animate() {
    static int lastAnim = -1;

    // Detect animation change
    if (canim != lastAnim) {
        cindex = 0;
        lastAnim = canim;
        lasttime = GetTime();
    }

    // Determine playback speed
    float speed = animSpeed;

    // Transition animations should be slower
    if (canim == IDLE_READY ||
        canim == LAND_IDLE ||
        canim == LAND_READY)
    {
        speed = transitionSpeed;
    }

    // Advance frames
    if (GetTime() - lasttime >= speed) {
        lasttime = GetTime();
        cindex++;
        if (cindex >= sizes[canim])
            cindex = 0;
    }
}
