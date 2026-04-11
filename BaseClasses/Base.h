#pragma once
#include <string>
#include <vector>
#include "raylib.h"

using namespace std;

// To store diff classes in same array
struct Node {
    int x, y;
    float gCost, hCost;
    Node* parent;
    float fCost() const { return gCost + hCost; }
};

enum HitboxType {
    ATTACKBOX,
    HURTBOX,
    TRIGGERBOX,
    RIDGIDBOX
};

struct Hitbox {
    Rectangle rect;
    string name;
    HitboxType type;
};

class HasCollider {
public:
    virtual ~HasCollider() = default; // Added virtual destructor
    vector<Hitbox> hitboxes;
    string tag;

    void AddHitbox(Rectangle r, const string& name, HitboxType type) {
       hitboxes.push_back({r, name, type});
    }
};

class Load
{
public:
    virtual ~Load() = default; // Added virtual destructor
    virtual void Draw() = 0;
};

class Listener
{
public:
    virtual ~Listener() = default; // Added virtual destructor
    virtual void OnEvent(string& command) = 0;
    virtual void OnSpecialEvent(string& command, vector<string> params) {};
};

class ChunkObject
{
public:
    virtual ~ChunkObject() = default; // Added virtual destructor
    virtual void Update() = 0;
    virtual void PlayerNear() = 0;
    virtual void PlayerFar() = 0;

    virtual void Startup() = 0;
    virtual void Shutdown() = 0;
};

struct AnimationFrames
{
    vector<Texture2D> run;
    vector<Texture2D> idle;
};

struct listenerload
{
    Listener* listener;
    string command;
};

enum NPCState
{
    NPC_NORMAL,
    NPC_SCARED,
    NPC_AGGRESSIVE,
    NPC_IDLE
};

class BaseNPC : public Load, public Listener, public HasCollider, public ChunkObject
{
protected:
    Vector2 chunkpos;
    string name;
    int health;

    int cframe = 0;
    float lasttime = 0.0f;
    int facingright = 1;
    NPCState state = NPCState::NPC_NORMAL;

    AnimationFrames hatframes;
    AnimationFrames headframes;
    AnimationFrames torsoframes;
    AnimationFrames armsframes;
    AnimationFrames legsframes;

    vector<AnimationFrames*> bodyframes = { &legsframes, &armsframes, &torsoframes, &headframes, &hatframes };
    vector<AnimationFrames*> draworder = { &torsoframes, &legsframes, &headframes, &armsframes, &hatframes };
public:
    virtual ~BaseNPC() = default; // Added virtual destructor
    Vector2 position;

    virtual void OnEvent(string& command) override = 0;
    virtual void OnSpecialEvent(string& command, vector<string> params) override {};

    virtual void Draw() override = 0;
    virtual void Update() override = 0;
    virtual void PlayerNear() override = 0;
    virtual void PlayerFar() override = 0;

    virtual void Shutdown() override = 0;
    virtual void Startup() override = 0;
};