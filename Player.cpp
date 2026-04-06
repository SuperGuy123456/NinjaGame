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
    allframes = SpriteSplitter::SplitByHorizontal(
    "../Art/Player/ninjaframes.png", 15, 10
);

    vector<Texture2D*> ptrs;
    ptrs.reserve(allframes.size());
    for (auto& f : allframes) ptrs.push_back(&f);


    // Helper to slice frames
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

    // IDLE (4)
    Animation idle;
    idle.frames = Slice(index, 4);
    idle.timePerFrame = 0.2f;
    idle.loop = true;
    idle.loopCount = -1;
    idle.reverse = false;
    idle.blockStates = false;
    animator.AddAnimationState("IDLE", idle);
    index += 4;

    // READYIDLE (2)
    Animation readyidle;
    readyidle.frames = Slice(index, 2);
    readyidle.timePerFrame = 0.2f;
    readyidle.loop = true;
    readyidle.loopCount = -1;
    readyidle.reverse = false;
    readyidle.blockStates = false;
    animator.AddAnimationState("READYIDLE", readyidle);
    index += 2;

    // IDLE_READY (6) — forward transition
    Animation idle_ready;
    idle_ready.frames = Slice(index, 6);
    idle_ready.timePerFrame = 0.35f;
    idle_ready.loop = false;
    idle_ready.loopCount = 0;
    idle_ready.reverse = false;
    idle_ready.blockStates = true;
    animator.AddAnimationState("IDLE_READY", idle_ready);
    index += 6;

    // READY_IDLE (reverse of IDLE_READY)
    Animation ready_idle = idle_ready;
    ready_idle.reverse = true;
    animator.AddAnimationState("READY_IDLE", ready_idle);

    // WALK (6)
    Animation walk;
    walk.frames = Slice(index, 6);
    walk.timePerFrame = 0.15f;
    walk.loop = true;
    walk.loopCount = -1;
    walk.reverse = false;
    walk.blockStates = false;
    animator.AddAnimationState("WALK", walk);
    index += 6;

    // READYWALK (6)
    Animation readywalk = walk;
    readywalk.frames = Slice(index, 6);
    animator.AddAnimationState("READYWALK", readywalk);
    index += 6;

    // RUN (6)
    Animation run;
    run.frames = Slice(index, 6);
    run.timePerFrame = 0.1f;
    run.loop = true;
    run.loopCount = -1;
    run.reverse = false;
    run.blockStates = false;
    animator.AddAnimationState("RUN", run);
    index += 6;

    // READYRUN (6)
    Animation readyrun = run;
    readyrun.frames = Slice(index, 6);
    animator.AddAnimationState("READYRUN", readyrun);
    index += 6;

    // ATTACK1 (3)
    Animation attack1;
    attack1.frames = Slice(index, 3);
    attack1.timePerFrame = 0.12f;
    attack1.loop = false;
    attack1.loopCount = 0;
    attack1.reverse = false;
    attack1.blockStates = true;
    animator.AddAnimationState("ATTACK1", attack1);
    index += 3;

    // ATTACK2 (4)
    Animation attack2;
    attack2.frames = Slice(index, 4);
    attack2.timePerFrame = 0.12f;
    attack2.loop = false;
    attack2.loopCount = 0;
    attack2.reverse = false;
    attack2.blockStates = true;
    animator.AddAnimationState("ATTACK2", attack2);
    index += 4;

    // UPJUMP (1)
    Animation upjump;
    upjump.frames = Slice(index, 1);
    upjump.timePerFrame = 0.2f;
    upjump.loop = false;
    upjump.loopCount = 0;
    upjump.reverse = false;
    upjump.blockStates = false;
    animator.AddAnimationState("UPJUMP", upjump);
    index += 1;

    // FALLING (1)
    Animation falling;
    falling.frames = Slice(index, 1);
    falling.timePerFrame = 0.2f;
    falling.loop = false;
    falling.loopCount = 0;
    falling.reverse = false;
    falling.blockStates = false;
    animator.AddAnimationState("FALLING", falling);
    index += 1;

    // READYUPJUMP (1)
    Animation readyupjump = upjump;
    readyupjump.frames = Slice(index, 1);
    animator.AddAnimationState("READYUPJUMP", readyupjump);
    index += 1;

    // READYFALLING (1)
    Animation readyfalling = falling;
    readyfalling.frames = Slice(index, 1);
    animator.AddAnimationState("READYFALLING", readyfalling);
    index += 1;

    // LAND_IDLE (2)
    Animation land_idle;
    land_idle.frames = Slice(index, 2);
    land_idle.timePerFrame = 0.25f;
    land_idle.loop = false;
    land_idle.loopCount = 0;
    land_idle.reverse = false;
    land_idle.blockStates = true;
    animator.AddAnimationState("LAND_IDLE", land_idle);
    index += 2;

    // LAND_READY (2)
    Animation land_ready = land_idle;
    land_ready.frames = Slice(index, 2);
    animator.AddAnimationState("LAND_READY", land_ready);
    index += 2;

    animator.ChangeAnimationState("IDLE");


    // Input listeners
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

Player::~Player() {}

bool Player::IsSolid(float wx, float wy) {
    return chunkmanager->CheckTileCollision({wx, wy}) != 0;
}

void Player::Update(double& dt) {

    // -----------------------------
    // INPUT
    // -----------------------------
    xchange = 0;
    if (IsKeyDown(KEY_A)) { xchange = -1; facing = -1; }
    if (IsKeyDown(KEY_D)) { xchange = 1; facing = 1; }

    // -----------------------------
    // PHYSICS
    // -----------------------------
    if (!grounded) {
        velocity.y += gravity * dt;
        if (velocity.y > maxFallSpeed) velocity.y = maxFallSpeed;
    } else {
        velocity.y = 0;
    }

    float currentSpeed = (isrunning ? 1000.0f : 600.0f);
    velocity.x = xchange * currentSpeed;

    const float TILE_SIZE = 8.0f;
    const float EPS = 0.1f;

    // MOVE X
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

    // MOVE Y
    pos.y += velocity.y * dt;

    bool wasGrounded = grounded;
    grounded = false;

    if (velocity.y >= 0) {
        if (IsSolid(pos.x - width/2 + EPS, pos.y + EPS) ||
            IsSolid(pos.x + width/2 - EPS, pos.y + EPS)) {
            pos.y = floor(pos.y / TILE_SIZE) * TILE_SIZE;
            velocity.y = 0;
            grounded = true;
        }
    } else {
        if (IsSolid(pos.x - width/2 + EPS, pos.y - height) ||
            IsSolid(pos.x + width/2 - EPS, pos.y - height)) {
            pos.y = ceil((pos.y - height) / TILE_SIZE) * TILE_SIZE + height;
            velocity.y = 0;
        }
    }

    // -----------------------------
    // ANIMATION STATE MACHINE
    // -----------------------------
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

    // Broadcast position
    playerposmanager.BroadcastSpecialMessage(
        "PLAYER_POS_UPDATE " + to_string(pos.x) + " " + to_string(pos.y)
    );

    animator.Animate(dt);
}

void Player::Draw() {
    Texture* tex = animator.GetTexture();

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

