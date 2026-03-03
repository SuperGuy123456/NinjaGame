//
// Created by tyagi on 3/1/2026.
//

#include "Player.h"

Player::Player(Vector2 pos, DrawLayer& _entitylayer,EventManager& _keyboardmanager) : entitylayer(_entitylayer), keyboardmanager(_keyboardmanager){
    this->pos = pos;

    entitylayer.AddDrawCall(this, 0);

    allframes = SpriteSplitter::SplitByHorizontal("../Art/Player/ninjaframes.png", 15, 10);
    TraceLog(LOG_INFO, "Player: Loaded %d frames", allframes.size());

    allsequences.push_back(&idle);          // 0
    allsequences.push_back(&readyidle);     // 1
    allsequences.push_back(&idle_ready);    // 2  ← moved here
    allsequences.push_back(&walk);          // 3
    allsequences.push_back(&readywalk);     // 4
    allsequences.push_back(&run);           // 5
    allsequences.push_back(&readyrun);      // 6
    allsequences.push_back(&attack1);       // 7
    allsequences.push_back(&attack2);       // 8
    allsequences.push_back(&upjump);        // 9
    allsequences.push_back(&falling);       // 10
    allsequences.push_back(&readyupjump);   // 11
    allsequences.push_back(&readyfalling);  // 12
    allsequences.push_back(&land_idle);     // 13
    allsequences.push_back(&land_ready);    // 14



    int fullindex = 0;

    for (int i = 0; i < NUMBEROFSTATES; ++i) {
        for (int j = 0; j < sizes[i]; ++j) {
            allsequences[i]->push_back(&allframes[fullindex]);
            fullindex++;
        }
    }
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

    //temporary
    grounded = true;
}

Player::~Player() {
    for (const Texture2D& tex : allframes) {
        UnloadTexture(tex);
    }
}

void Player::Draw() {
    Texture2D* tex = (*allsequences[canim])[cindex];

    // Source rectangle: full texture
    Rectangle src = { 0.0f, 0.0f, facing * (float)tex->width, (float)tex->height };

    float offset = 10.0f * facing; // push left when facing right, push right when facing left

    Rectangle dest = {
        pos.x + offset,
        pos.y,
        (float)tex->width,
        (float)tex->height
    };


    // Origin: pivot point (center)
    Vector2 origin = { tex->width / 2.0f, tex->height / 2.0f };

    // Rotation: none
    float rotation = 0.0f;

    DrawTexturePro(*tex, src, dest, origin, rotation, WHITE);
}



void Player::Update(double& dt) {
    Animate();

    // Detect ready toggles
    bool justBecameReady = (ready && !wasready);
    bool justStoppedReady = (!ready && wasready);

    // Forward transition: idle -> ready
    if (justBecameReady) {
        playingTransition = true;
        playingReverseTransition = false;
        canim = IDLE_READY;
        cindex = 0; // start forward
    }

    // Reverse transition: ready -> idle
    if (justStoppedReady) {
        playingReverseTransition = true;
        playingTransition = false;
        canim = IDLE_READY;
        cindex = sizes[IDLE_READY] - 1; // start from last frame
    }

    // If playing forward transition
    if (playingTransition) {
        if (cindex == sizes[IDLE_READY] - 1) {
            playingTransition = false;
            canim = READYIDLE;
            cindex = 0;
        }
        wasready = ready;
        return; // block movement animations
    }

    // If playing reverse transition
    if (playingReverseTransition) {
        if (cindex == 0) {
            playingReverseTransition = false;
            canim = IDLE;
            cindex = 0;
        }
        wasready = ready;
        return; // block movement animations
    }

    // Movement (only when not in transition)
    if (!playingTransition && !playingReverseTransition) {

        float speed = 0.0f;

        if (xchange != 0 && grounded && !justjumped) {
            if (isrunning) {
                speed = ready ? READYRUNSPEED : RUNSPEED;
            } else {
                speed = ready ? READYSPEED : WALKSPEED;
            }
        }

        pos.x += speed * xchange * dt;
    }


    // Normal animation logic
    if (xchange == -1) facing = -1;
    if (xchange == 1)  facing = 1;

    if (xchange == 0 && grounded && !justjumped) {
        if (ready) canim = READYIDLE;
        else       canim = IDLE;
    }

    if (isrunning && xchange != 0 && grounded && !justjumped) {
        if (ready) canim = READYRUN;
        else       canim = RUN;
    }
    else if (xchange != 0 && grounded && !justjumped) {
        if (ready) canim = READYWALK;
        else       canim = WALK;
    }

    wasready = ready;
}



void Player::OnEvent(string &command)
{
    // HOLD_ events
    if (command.rfind("HOLD_", 0) == 0)
    {
        string key = command.substr(5); // remove "HOLD_


        if (key == "W") {
            /* jump */
            if (grounded) {
                grounded = false;
                justjumped = true;
            }
        }
        if (key == "A") {
            /* move left */
            xchange = -1;
        }
        if (key == "D") {
            /* move right */
            xchange = 1;
        }
        if (key == "SHIFT") {
            isrunning = true;
        }

        return;
    }
    // PRESS_ events
    else if (command.rfind("PRESS_", 0) == 0)
    {
        string key = command.substr(6); // remove "PRESS_"

        if (key == "E") {
            if (ready) {
                ready = false;
            }
            else {
                ready = true;
            }
        }

        return;
    }
    else if (command.rfind("RELEASE_", 0) == 0) {
        string key = command.substr(8);

        if (key == "W") {
            /* jump */
            //idk why i made this a hold key...
        }
        if (key == "A") {
            /* move left */
            xchange = 0;
        }
        if (key == "D") {
            /* move right */
            xchange = 0;
        }
        if (key == "SHIFT") {
            isrunning = false;
        }

        return;
    }
    // Other events (non-input)
    cout << "Other event: " << command << endl;
}


void Player::OnSpecialEvent(string &command, vector<string> params) {

}

void Player::Animate() {
    static int lastAnim = -1;

    // Reset frame index when animation changes
    if (canim != lastAnim) {
        if (playingReverseTransition)
            cindex = sizes[canim] - 1;
        else
            cindex = 0;

        lastAnim = canim;
    }

    // Frame timing
    if (GetTime() - lasttime >= 0.2f) {
        lasttime = GetTime();

        // Forward transition
        if (playingTransition) {
            cindex++;
            return;
        }

        // Reverse transition
        if (playingReverseTransition) {
            cindex--;
            return;
        }

        // Normal animation
        cindex++;
        if (cindex >= sizes[canim]) {
            cindex = 0;
        }
    }
}



