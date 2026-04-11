#pragma once
#include "raylib.h"
#include <iostream>
class GameCamera {
public:
    static void Init(Vector2* target);
    static void Update(float dt);

    static void TriggerShake(float intensity, float duration);
    static void PulseZoom(float amount, float duration);

    static Camera2D GetCamera();
    static Vector2* target;
private:
    static Camera2D cam;


    // Shake
    static float shakeTime;
    static float shakeIntensity;
    static Vector2 shakeOffset;

    // Zoom pulse
    static float baseZoom;
    static float zoom;
    static float zoomPulseTime;
    static float zoomPulseDuration;
    static float zoomStart;
    static float zoomEnd;
};
