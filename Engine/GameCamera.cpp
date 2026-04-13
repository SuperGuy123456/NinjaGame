#include "GameCamera.h"
#include "screenres.h"
#include <cmath>

#include "ReferencePool.h"

// Static definitions
Camera2D GameCamera::cam = {0};
Vector2 *GameCamera::target = nullptr;

float GameCamera::shakeTime = 0.0f;
float GameCamera::shakeIntensity = 0.0f;
Vector2 GameCamera::shakeOffset = {0, 0};

float GameCamera::baseZoom = 1.0f;
float GameCamera::zoom = 1.0f;
float GameCamera::zoomPulseTime = 0.0f;
float GameCamera::zoomPulseDuration = 0.0f;
float GameCamera::zoomStart = 1.0f;
float GameCamera::zoomEnd = 1.0f;

bool GameCamera::usingFreeCam = false;
bool GameCamera::loadAroundCam = false;
bool GameCamera::chunkloadingState = true;
Vector2 GameCamera::freeCamPos = {0, 0};
float GameCamera::speed = 300;

void GameCamera::Init(Vector2 *t) {
    target = t;

    cam.target = {0, 0};
    cam.offset = {0, 0};
    cam.rotation = 0.0f;
    cam.zoom = baseZoom;
    zoom = baseZoom;
}

void GameCamera::TriggerShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeTime = duration;
}

void GameCamera::PulseZoom(float amount, float duration) {
    zoomStart = baseZoom; // always start from base
    zoom = baseZoom; // reset current zoom so pulses stack
    zoomEnd = amount;
    zoomPulseDuration = duration;
    zoomPulseTime = duration;
}

void GameCamera::Update(float dt) {
    if (usingFreeCam) {
        float speed = GameCamera::speed * dt;
        if (IsKeyDown(KEY_LEFT_SHIFT)) speed *= 2;

        if (IsKeyDown(KEY_W)) freeCamPos.y -= speed;
        if (IsKeyDown(KEY_S)) freeCamPos.y += speed;
        if (IsKeyDown(KEY_A)) freeCamPos.x -= speed;
        if (IsKeyDown(KEY_D)) freeCamPos.x += speed;

        cam.target = freeCamPos;
        cam.zoom = zoom;
        if (loadAroundCam) {
            EventManager* playerposmanager = (EventManager*)ReferencePool::Get("PLAYERPOSMANAGER REF");
            playerposmanager->BroadcastSpecialMessage("PLAYER_POS_UPDATE " + to_string(freeCamPos.x) + " " + to_string(freeCamPos.y));
        }
        return; // skip player follow
    }
    else {
        freeCamPos = cam.target;
    }

    // ---------------------------
    // Smooth follow
    // ---------------------------
    if (target) {
        float lerp = 0.15f;

        // 1. Calculate the NEW float position (don't snap yet!)
        // Use a temporary variable to keep the movement fluid
        float nextX = cam.target.x + (target->x - cam.target.x) * lerp;
        float nextY = cam.target.y + (target->y - cam.target.y) * lerp;

        // 2. APPLY THE SNAP HERE
        // This ensures the internal cam.target is ALWAYS a whole number
        cam.target.x = std::round(nextX);
        cam.target.y = std::round(nextY);
    }

    // ---------------------------
    // Screen shake
    // ---------------------------
    if (shakeTime > 0.0f) {
        shakeTime -= dt;

        float falloff = (shakeTime > 0.0f) ? shakeTime : 0.0f;

        float rx = (float) GetRandomValue(-100, 100) / 100.0f;
        float ry = (float) GetRandomValue(-100, 100) / 100.0f;

        shakeOffset.x = rx * shakeIntensity * falloff;
        shakeOffset.y = ry * shakeIntensity * falloff;

        if (shakeTime <= 0.0f) {
            shakeOffset = {0, 0};
        }
    }

    // ---------------------------
    // Zoom pulse (debug version)
    // ---------------------------
    if (zoomPulseTime > 0.0f) {
        zoomPulseTime -= dt;

        float t = 1.0f - (zoomPulseTime / zoomPulseDuration);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        // Ease-in-out cubic
        auto EaseInOutCubic = [](float x) {
            return (x < 0.5f)
                       ? 4.0f * x * x * x
                       : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;
        };

        // Build symmetric arc: 0 → 1 → 0
        float arc;
        if (t < 0.5f) {
            arc = EaseInOutCubic(t * 2.0f); // zoom in
        } else {
            arc = EaseInOutCubic((1.0f - t) * 2.0f); // zoom out
        }

        zoom = zoomStart + (zoomEnd - zoomStart) * arc;

        // DEBUG PRINT
        std::cout
                << "[ZOOM] t=" << t
                << " arc=" << arc
                << " zoom=" << zoom
                << " timeLeft=" << zoomPulseTime
                << std::endl;

        if (zoomPulseTime <= 0.0f) {
            zoom = baseZoom;
            std::cout << "[ZOOM] Reset to base zoom\n";
        }
    }

    cam.zoom = zoom;

    // Use the SAME size as the render texture
    float vWidth  = SCREENWIDTH  / 10.0f;   // or read from ReferencePool
    float vHeight = SCREENHEIGHT / 10.0f;

    cam.offset = Vector2{ std::floor(vWidth * 0.5f + shakeOffset.x),
                          std::floor(vHeight * 0.5f + shakeOffset.y) };
}

Camera2D GameCamera::GetCamera() {
    return cam;
}
