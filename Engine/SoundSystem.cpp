//
// Created by Manomay Tyagi on 4/11/26.
//

#include "SoundSystem.h"

map<string, SoundSource> SoundSystem::sources;
map<float, Sound> SoundSystem::bgsounds;

void SoundSystem::AddBgMusic(Sound sound, float volume) {
    SoundSystem::bgsounds[volume] = sound;
}

void SoundSystem::AddSoundSource(SoundSource soundsource, string id) {
    SoundSystem::sources[id] = soundsource;
}

void SoundSystem::RemoveSoundSource(string id) {
    SoundSystem::sources.erase(id);
}

void SoundSystem::Update() {
    Player* playerptr = (Player*)ReferencePool::Get("PLAYER REF");
    if (!playerptr) return; // Safety check

    Vector2 playerPos = playerptr->pos;

    for (auto& [id, source] : sources) {
        float distance = Vector2Distance(playerPos, source.pos);

        if (distance <= (float)source.soundradius) {
            // 1. Calculate how close the player is (0.0 at edge, 1.0 at center)
            // We use (1.0 - fraction) so volume increases as distance decreases
            float closeness = 1.0f - (distance / (float)source.soundradius);

            // 2. Map closeness to the min/max volume range
            float volume = source.minvolume + (closeness * (source.maxvolume - source.minvolume));

            SetSoundVolume(source.sound, volume);

            // 3. Play if not already playing (assuming looping or persistent ambient sounds)
            if (!IsSoundPlaying(source.sound)) {
                PlaySound(source.sound);
            }
        } else {
            // Optional: Fade out or stop sound if player leaves the radius
            if (IsSoundPlaying(source.sound)) {
                StopSound(source.sound);
            }
        }
    }

    for (auto& [volume, sound] :  SoundSystem::bgsounds) {
        if (!IsSoundPlaying(sound)) {
            SetSoundVolume(sound, volume);
            PlaySound(sound);
        }

    }


}