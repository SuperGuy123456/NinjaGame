#pragma once
#include "raylib.h"
#include <iostream>
#include "Engine/EventManager.h"
class GameCamera {
public:
    static void Init(Vector2* target);
    static void Update(float dt);

    static void TriggerShake(float intensity, float duration);
    static void PulseZoom(float amount, float duration);

    static Camera2D GetCamera();
    static Vector2* target;
    static bool usingFreeCam;
    static bool loadAroundCam;
    static bool chunkloadingState;
    static float speed;

    static float zoom;

private:
    static Camera2D cam;
    static Vector2 freeCamPos;


    // Shake
    static float shakeTime;
    static float shakeIntensity;
    static Vector2 shakeOffset;

    // Zoom pulse
    static float baseZoom;
    static float zoomPulseTime;
    static float zoomPulseDuration;
    static float zoomStart;
    static float zoomEnd;
};
