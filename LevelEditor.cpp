#include "LevelEditor.h"
#include "ImGuiFileDialog.h"

static const int RULE_TILES[3][3] = {
    { 0, 1, 2 },
    { 5, 6, 7 },
    {10,11,12}
};

LevelEditor::LevelEditor(ChunkManager* cm)
    : chunkManager(cm)
{
    alltiles = SpriteSplitter::SplitGrid("../Art/Tilemap/Tilemap.png", 8, 8, 4, 5, 3);
}

LevelEditor::~LevelEditor() {
    for (Texture2D& tex : alltiles)
        UnloadTexture(tex);
}
void LevelEditor::UpdateCollisionNeighbors(int cx, int cy, int lx, int ly)
{
    const int dirs[4][2] = {
        {0,-1}, {0,1}, {-1,0}, {1,0}
    };

    for (auto& d : dirs) {
        int nx = lx + d[0];
        int ny = ly + d[1];

        int ncx = cx;
        int ncy = cy;

        if (nx < 0) { nx += TILESPERCHUNK; ncx--; }
        if (ny < 0) { ny += TILESPERCHUNK; ncy--; }
        if (nx >= TILESPERCHUNK) { nx -= TILESPERCHUNK; ncx++; }
        if (ny >= TILESPERCHUNK) { ny -= TILESPERCHUNK; ncy++; }

        if (!chunkManager->ValidChunk(ncx, ncy))
            continue;

        Vector2 wp = TileToWorld(ncx, ncy, nx, ny);
        int id = chunkManager->CheckTileId(wp, 1);

        if (id == -1) {
            chunkManager->ModifyCollData(wp, 0);
            continue;
        }

        if (id == 15) {
            chunkManager->ModifyCollData(wp, 0);
            continue;
        }

        if (chunkManager->TileTouchesAir(wp, 1))
            chunkManager->ModifyCollData(wp, 1);
        else
            chunkManager->ModifyCollData(wp, 0);
    }
}


bool LevelEditor::IsRuleTile(int id) {
    if (id == 16) return true;
    return (id == 0 || id == 1 || id == 2 ||
            id == 5 || id == 6 || id == 7 ||
            id == 10|| id == 11|| id == 12);
}

Vector2 LevelEditor::TileToWorld(int cx, int cy, int lx, int ly) {
    float wx = (cx * TILESPERCHUNK + lx) * TILESIZE * PIXELSCALE;
    float wy = (cy * TILESPERCHUNK + ly) * TILESIZE * PIXELSCALE;
    return {wx, wy};
}

int LevelEditor::GetTile(int cx, int cy, int lx, int ly) {
    if (lx < 0) { lx += TILESPERCHUNK; cx--; }
    if (ly < 0) { ly += TILESPERCHUNK; cy--; }
    if (lx >= TILESPERCHUNK) { lx -= TILESPERCHUNK; cx++; }
    if (ly >= TILESPERCHUNK) { ly -= TILESPERCHUNK; cy++; }

    if (!chunkManager->ValidChunk(cx, cy))
        return -1;

    Vector2 wp = TileToWorld(cx, cy, lx, ly);
    return chunkManager->CheckTileId(wp, 1);
}

int LevelEditor::ComputeRuleTileVariant(int cx, int cy, int lx, int ly) {
    int up    = GetTile(cx, cy, lx, ly - 1);
    int down  = GetTile(cx, cy, lx, ly + 1);
    int left  = GetTile(cx, cy, lx - 1, ly);
    int right = GetTile(cx, cy, lx + 1, ly);

    bool upC    = IsRuleTile(up);
    bool downC  = IsRuleTile(down);
    bool leftC  = IsRuleTile(left);
    bool rightC = IsRuleTile(right);

    int row = 1;
    int col = 1;

    if (!upC)    row = 0;
    if (!downC)  row = 2;
    if (!leftC)  col = 0;
    if (!rightC) col = 2;

    return RULE_TILES[row][col];
}

void LevelEditor::UpdateNeighbors(int cx, int cy, int lx, int ly) {
    const int dirs[4][2] = {
        {0,-1}, {0,1}, {-1,0}, {1,0}
    };

    for (auto& d : dirs) {
        int nx = lx + d[0];
        int ny = ly + d[1];

        int ncx = cx;
        int ncy = cy;

        if (nx < 0) { nx += TILESPERCHUNK; ncx--; }
        if (ny < 0) { ny += TILESPERCHUNK; ncy--; }
        if (nx >= TILESPERCHUNK) { nx -= TILESPERCHUNK; ncx++; }
        if (ny >= TILESPERCHUNK) { ny -= TILESPERCHUNK; ncy++; }

        if (!chunkManager->ValidChunk(ncx, ncy))
            continue;

        Vector2 wp = TileToWorld(ncx, ncy, nx, ny);
        int id = chunkManager->CheckTileId(wp, 1);

        if (IsRuleTile(id)) {
            int newTile = ComputeRuleTileVariant(ncx, ncy, nx, ny);
            chunkManager->ModifyTexData(wp, 1, newTile);
        }
    }
}

void LevelEditor::DrawWorldGizmos() {
    if (!enabled) return;

    for (auto& [id, src] : SoundSystem::sources) {
        Color c = (id == selectedSoundID) ? RED : ORANGE;

        // Draw radius
        DrawCircleLines(src.pos.x, src.pos.y, src.soundradius, Fade(c, 0.4f));

        // Draw center icon
        DrawCircle(src.pos.x, src.pos.y, 6, c);
    }
}


void LevelEditor::Update(float dt) {
    if (!enabled) return;
    HandleInput();
}

void LevelEditor::HandleInput() {
    if (!enabled) return;
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (currentTool == EditorTool::Tile ||
    currentTool == EditorTool::Erase ||
    currentTool == EditorTool::Grass)
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 worldPos = GetMouseWorldPos();
            ApplyToolAt(worldPos);
        }
    }
    else
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 worldPos = GetMouseWorldPos();
            ApplyToolAt(worldPos);
        }
    }

}

Vector2 LevelEditor::GetMouseWorldPos() {
    Vector2 mouse = GetMousePosition();
    mouse.x /= 10;
    mouse.y /= 10;
    return GetScreenToWorld2D(mouse, GameCamera::GetCamera());
}

void LevelEditor::SelectSoundAt(Vector2 worldPos) {
    float clickRadius = 12.0f;

    selectedSoundID = "";

    for (auto& [id, src] : SoundSystem::sources) {
        float dx = worldPos.x - src.pos.x;
        float dy = worldPos.y - src.pos.y;
        float dist = sqrtf(dx*dx + dy*dy);

        if (dist <= clickRadius) {
            selectedSoundID = id;
            return;
        }
    }
}


void LevelEditor::ApplyToolAt(Vector2 worldPos) {
    unSaved = true;
    int tileX, tileY;
    chunkManager->WorldToTile(worldPos, tileX, tileY);

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

    switch (currentTool) {

        case EditorTool::Tile: {
            if (selectedTile == 16) {
                int realTile = ComputeRuleTileVariant(cx, cy, lx, ly);
                chunkManager->ModifyTexData(worldPos, currentLayer, realTile);
                UpdateNeighbors(cx, cy, lx, ly);
            }
            else {
                chunkManager->ModifyTexData(worldPos, currentLayer, selectedTile);
            }

            if (chunkManager->TileTouchesAir(worldPos, currentLayer) and currentLayer == 1)
                chunkManager->ModifyCollData(worldPos, 1);
            UpdateCollisionNeighbors(cx, cy, lx, ly);
            break;
        }

        case EditorTool::Erase: {
            // Erase = set tile to -1
            chunkManager->ModifyTexData(worldPos, currentLayer, -1);

            // If erasing on main layer, remove collision
            if (currentLayer == 1)
                chunkManager->ModifyCollData(worldPos, 0);

            // If erasing a rule tile, neighbors must update
            UpdateNeighbors(cx, cy, lx, ly);
            UpdateCollisionNeighbors(cx, cy, lx, ly);
            break;
        }
        case EditorTool::Grass: {
            chunkManager->SpawnGrassAtWorld(worldPos);
            chunkManager->ModifyCollData(worldPos, 3);
            break;
        }
        case EditorTool::Sound: {
            SoundSource src;
            src.pos = worldPos;
            src.soundPath = "../Audio/Fight/HardSmack.wav";   // store path
            src.sound = LoadSound(src.soundPath.c_str());

            src.soundradius = 15;
            src.minvolume = 0.2f;
            src.maxvolume = 1.0f;

            string id = "sound_" + std::to_string(rand());
            SoundSystem::AddSoundSource(src, id);

            selectedSoundID = id;
            break;
        }
        case EditorTool::Edit: {
            SelectSoundAt(worldPos);
            break;
        }


        default:
            break;
    }
}


void LevelEditor::DrawGUI() {
    if (!enabled) return;

    rlImGuiBegin();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2((float)GetScreenWidth(), 60), ImGuiCond_Always);
    ImGui::Begin("Tools", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    auto ToolButton = [&](const char* label, EditorTool tool) {
        bool selected = (currentTool == tool);
        if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1, 1));
        if (ImGui::Button(label)) currentTool = tool;
        if (selected) ImGui::PopStyleColor();
        ImGui::SameLine();
    };

    ToolButton("Tile", EditorTool::Tile);
    ToolButton("Erase", EditorTool::Erase);
    ToolButton("Grass", EditorTool::Grass);
    ToolButton("Spawner", EditorTool::Spawner);
    ToolButton("Sound", EditorTool::Sound);
    ToolButton("Edit", EditorTool::Edit);

    ImGui::SameLine();
    ImGui::Checkbox("Gizmos", &showGizmos);
    ImGui::SameLine();
    ImGui::Checkbox("God Mode", &GameCamera::usingFreeCam);
    ImGui::SameLine();
    if (GameCamera::usingFreeCam) {
        ImGui::Checkbox("Load Around Free Camera", &GameCamera::loadAroundCam);
        ImGui::SameLine();
        ImGui::Checkbox("Unloading", &GameCamera::chunkloadingState);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Cam Zoom");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##CamZoom", &GameCamera::zoom, 0.0f, 5.0f);

        ImGui::SameLine();

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Cam Speed");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##CamSpeed", &GameCamera::speed, 0.0f, 1000.0f);
        ImGui::SameLine();
    }
    if (ImGui::Button("SAVE LVL")) {
        chunkManager->SaveLevel();
        unSaved = false;
    }

    if (unSaved) {
        ImGui::SameLine(0, 5.0f); // 5px spacing
        // Get the cursor position to draw the dot
        ImVec2 pos = ImGui::GetCursorScreenPos();
        pos.y += ImGui::GetFontSize() * 0.5f; // Center it vertically to the text

        // Draw a small orange/red dot
        ImGui::GetWindowDrawList()->AddCircleFilled(pos, 4.0f, IM_COL32(255, 165, 0, 255));

        // Advance the dummy cursor so the next item doesn't overlap the dot
        ImGui::Dummy(ImVec2(8.0f, 0.0f));
    }

    ImGui::End();

    float rightW = 320;
    float topH = 40;
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    ImGui::SetNextWindowPos(ImVec2(screenW - rightW, topH), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightW, screenH * 0.55f), ImGuiCond_Always);
    ImGui::Begin("Palette", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    ImGui::Text("Layers:");
    ImGui::RadioButton("Backlayer", &currentLayer, 0);
    ImGui::RadioButton("Main Layer", &currentLayer, 1);
    ImGui::RadioButton("Foreground Layer", &currentLayer, 2);

    ImGui::Separator();
    ImGui::Text("Tiles:");

    int tilesPerRow = 5;

    for (int i = 0; i < alltiles.size(); i++) {
        Texture2D& tex = alltiles[i];
        std::string id = "tile_" + std::to_string(i);

        if (rlImGuiImageButton(id.c_str(), &tex))
            selectedTile = i;

        if (selectedTile == i) {
            ImGui::GetWindowDrawList()->AddRect(
                ImGui::GetItemRectMin(),
                ImGui::GetItemRectMax(),
                IM_COL32(255, 255, 0, 255),
                2.0f
            );
        }

        if ((i + 1) % tilesPerRow != 0)
            ImGui::SameLine();
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(screenW - rightW, topH + screenH * 0.55f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightW, screenH * 0.45f), ImGuiCond_Always);
    ImGui::Begin("Details", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    ImGui::Text("Tool: %d", (int)currentTool);
    ImGui::Separator();

    if (currentTool == EditorTool::Tile)
        ImGui::Text("Selected Tile: %d", selectedTile);

    ImGui::Separator();

    if ((currentTool == EditorTool::Sound || currentTool == EditorTool::Edit)
    && selectedSoundID != "")
    {
        SoundSource& src = SoundSystem::sources[selectedSoundID];

        ImGui::Separator();
        ImGui::Text("Sound Source Settings");

        // Show current path
        ImGui::Text("Path:");
        ImGui::SameLine();
        ImGui::Text("%s", src.soundPath.c_str());

        // Button to open file dialog
        if (ImGui::Button("Choose Sound File")) {
            IGFD::FileDialogConfig config;
            if (!src.soundPath.empty()) {
                config.filePathName = src.soundPath;
            } else {
                config.path = ".";
            }
            ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseSoundDlg",
                "Select Sound File",
                ".wav,.ogg,.mp3",
                config);
        }

        // File dialog handling
        if (ImGuiFileDialog::Instance()->Display("ChooseSoundDlg")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

                // Update sound
                src.soundPath = filePath;
                UnloadSound(src.sound);
                src.sound = LoadSound(src.soundPath.c_str());
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // Radius + volume sliders
        ImGui::SliderInt("Radius", &src.soundradius, 10, 1000);
        ImGui::SliderFloat("Min Volume", &src.minvolume, 0.0f, src.maxvolume);
        ImGui::SliderFloat("Max Volume", &src.maxvolume, src.minvolume, 100.0f);

        if (ImGui::Button("Delete Sound")) {
            SoundSystem::RemoveSoundSource(selectedSoundID);
            selectedSoundID = "";
        }
    }




    ImGui::End();

    rlImGuiEnd();
}
