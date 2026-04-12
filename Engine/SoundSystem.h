//
// Created by Manomay Tyagi on 4/11/26.
//

#ifndef NINJAGAME_SOUNDSYSTEM_H
#define NINJAGAME_SOUNDSYSTEM_H
#include "raylib.h"
#include "Engine/ReferencePool.h"
#include "Player.h"

struct SoundSource {
    Vector2 pos;
    Sound sound;

    int soundradius;
    float minvolume;
    float maxvolume;
};

class SoundSystem {
public:
    static void AddSoundSource(SoundSource soundsource, string id);
    static void RemoveSoundSource(string id);
    static void Update();
private:

    static map<string, SoundSource> sources;
};

#endif //NINJAGAME_SOUNDSYSTEM_H