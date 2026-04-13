#pragma once
#include "raylib.h"
#include "Engine/Chunks.h"
#include "imgui.h"
#include "rlImGui.h"
#include "Engine/GameCamera.h"
#include "Engine/SoundSystem.h"

enum class EditorTool {
    None,
    Tile,
    Erase,
    Grass,
    Spawner,
    Sound,
    Edit,
};

class LevelEditor {
public:
    LevelEditor(ChunkManager* chunkManager);
    ~LevelEditor();

    void Update(float dt);
    void DrawGUI();     // ImGui windows
    void DrawWorldGizmos();
    void HandleInput(); // mouse + keyboard

    bool enabled = true;
    bool showGizmos = true;

    bool IsRuleTile(int id);
    int ComputeRuleTileVariant(int cx, int cy, int lx, int ly);

private:
    ChunkManager* chunkManager;

    // UI state
    EditorTool currentTool = EditorTool::Tile;
    int currentLayer = 0;
    int selectedTile = 0;

    // Internal helpers
    Vector2 GetMouseWorldPos();
    void ApplyToolAt(Vector2 worldPos);
    void UpdateNeighbors(int cx, int cy, int lx, int ly);
    int GetTile(int cx, int cy, int lx, int ly);
    Vector2 TileToWorld(int cx, int cy, int lx, int ly);
    void UpdateCollisionNeighbors(int cx, int cy, int lx, int ly);
    void SelectSoundAt(Vector2 worldPos);

    vector<Texture2D> alltiles;
    string selectedSoundID;

    // Indexes in your tilesheet


};
