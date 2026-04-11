#include "Grass.h"
#include "Player.h"

RenderTexture2D Grass::sharedTarget;
int Grass::virtualHeight;
int Grass::virtualWidth;
int Grass::scale;

Grass::Grass(Vector2& _pos) : pos(_pos) {

    pos.x = pos.x/10;
    pos.y = pos.y/10;
    int localX = 0;
    for (int i =0; i<count+1; i++) {
        int height = GetRandomValue(min_height, max_height);
        int tipoffset = GetRandomValue(min_tipoffset, max_tipoffset);
        Triangle tri = {Vector2{localX + pos.x, pos.y + height}, Vector2{localX+pos.x+width, pos.y + height}, Vector2{pos.x+localX+tipoffset+width/2, pos.y}, Vector2{0, 0}, basecolor};
        blades.push_back(tri);
        localX += spacing;
    }

    //DrawLayer* bglayer = (DrawLayer*)ReferencePool::Get("BGLAYER REF");
    //bglayer->AddDrawCall(this, 0);

}

void Grass::Init() {
    Grass::virtualHeight = SCREENHEIGHT/10;
    Grass::virtualWidth = SCREENWIDTH/10;
    Grass::scale = 10;
    Grass::sharedTarget = LoadRenderTexture(virtualWidth, virtualHeight);
}

void Grass::DeInit() {
    UnloadRenderTexture(Grass::sharedTarget);
}

Grass::~Grass() {

}

void Grass::EndRender() {
    EndTextureMode();
    // 2. Draw the tiny canvas to the actual screen, scaled up
    // Note: Source rectangle Y is negative because OpenGL textures are upside down
    Rectangle source = { 0, 0, (float)Grass::sharedTarget.texture.width, (float)-Grass::sharedTarget.texture.height };
    Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };

    DrawTexturePro(Grass::sharedTarget.texture, source, dest, Vector2{0, 0}, 0.0f, WHITE);
}

void Grass::BeginRender() {
    BeginTextureMode(Grass::sharedTarget);
    ClearBackground(BLANK); // Keep it transparent
}

void Grass::Draw(Vector2 cameraPos) {
    // 1. Convert the Camera's World Position to "Grass Space"
    float camX_scaled = cameraPos.x / 10.0f;
    float camY_scaled = cameraPos.y / 10.0f;

    // 2. Find the center of your low-res texture
    float halfWidth = (float)Grass::virtualWidth / 2.0f;
    float halfHeight = (float)Grass::virtualHeight / 2.0f;

    for (Triangle& tri : blades) {
        /* The Logic:
           (Blade Position) - (Camera Position) = Relative distance from camera.
           Relative distance + (Texture Center) = Position on the texture.
        */
        Vector2 p1 = { (tri.p1.x - camX_scaled) + halfWidth, (tri.p1.y - camY_scaled) + halfHeight };
        Vector2 p2 = { (tri.p2.x - camX_scaled) + halfWidth, (tri.p2.y - camY_scaled) + halfHeight };
        Vector2 p3 = { (tri.p3.x - camX_scaled) + halfWidth, (tri.p3.y - camY_scaled) + halfHeight };

        DrawTriangle(p1, p2, p3, tri.color);
    }
}

void Grass::Update() {
    //do the update stuff
}
