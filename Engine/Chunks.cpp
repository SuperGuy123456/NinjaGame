#include "Chunks.h"
Chunk::Chunk(int cx, int cy,const vector<vector<int>>& _bgtexdata, const vector<vector<int> >& _maintexdata, const vector<vector<int>>& _foretexdata, const vector<vector<int> > &col, const vector<Texture2D>& _tiletexs, DrawLayer& _bglayer) : bgtexdata(_bgtexdata), maintexdata(_maintexdata), foretexdata(_foretexdata), colData(col), tiletexs(_tiletexs), bglayer(_bglayer) {
    x = cx;
    y = cy;

    BuildColliders();

}

Chunk::~Chunk(){}; //no need to unload textures as they are part of chunkmanager

void Chunk::BuildColliders() {
    // CLEAR OLD COLLIDERS FIRST
    Collision::RemoveCollider(this);
    hitboxes.clear();
    grasstileposes.clear();
    grassobjs.clear();
    //make all of the boxes
    int localY = 0;
    for (const auto& row : colData) {
        int localX = 0;

        for (int id : row) {
            if (id != 0 and id != 3) {
                AddHitbox(
                    Rectangle{
                    (float)x * TILESIZE * COLLIDERSCALE * TILESPERCHUNK + localX * TILESIZE * COLLIDERSCALE,
                    (float)y * TILESIZE * COLLIDERSCALE * TILESPERCHUNK + localY * TILESIZE * COLLIDERSCALE,TILESIZE * COLLIDERSCALE, TILESIZE * COLLIDERSCALE},
                "TILE AT: " + to_string(x) + "," + to_string(y),
                RIDGIDBOX
                );

            }
            if (id == 3) {
                //grass detected (spawn some grass)
                grasstileposes.push_back({(float)x * TILESIZE * PIXELSCALE * TILESPERCHUNK + localX * TILESIZE * PIXELSCALE,
                    (float)y * TILESIZE * PIXELSCALE * TILESPERCHUNK + localY * TILESIZE * PIXELSCALE});
                grassobjs.push_back(new Grass(grasstileposes.back()));
            }
            localX++;
        }
        localY++;
    }
    Collision::AddCollider(this);
}

void Chunk::SpawnGrassAt(int lx, int ly)
{
    float wx = (x * TILESPERCHUNK + lx) * TILESIZE * PIXELSCALE;
    float wy = (y * TILESPERCHUNK + ly) * TILESIZE * PIXELSCALE;

    Vector2 pos = { wx, wy };

    grasstileposes.push_back(pos);
    grassobjs.push_back(new Grass(grasstileposes.back()));
}


void Chunk::Update() {
    //TODO for everything inside the chunk
    for (Grass* grassobj : grassobjs) {
        grassobj->Update();
    }
}
void Chunk::DrawTexLayer(const std::vector<std::vector<int>>& layer) {
    int localY = 0;
    for (const auto& row : layer) {
        int localX = 0;
        for (int id : row) {
            if (id >= 0) { // skip empty
                DrawTexture(
                    tiletexs[id],
                    x * TILESIZE * PIXELSCALE * TILESPERCHUNK + localX * TILESIZE * PIXELSCALE,
                    y * TILESIZE * PIXELSCALE * TILESPERCHUNK + localY * TILESIZE * PIXELSCALE,
                    WHITE
                );
            }
            localX++;
        }
        localY++;
    }
}

void Chunk::Draw() {
    int localY = 0;

    if (DEBUG_COLLISION) {
        for (const auto& row : colData) {
            int localX = 0;

            for (int id : row) {
                if (id != 0) {
                    DrawRectangle(
                        x * TILESIZE * COLLIDERSCALE * TILESPERCHUNK + localX * TILESIZE * COLLIDERSCALE,
                        y * TILESIZE * COLLIDERSCALE * TILESPERCHUNK + localY * TILESIZE * COLLIDERSCALE,
                        TILESIZE * COLLIDERSCALE,
                        TILESIZE * COLLIDERSCALE,
                        Color{255, 255, 255, 80}
                    );
                }
                localX++;
            }
            localY++;
        }
    }
    else {
        for (Grass* grassobj : grassobjs) {
            grassobj->Draw();
        }
        DrawTexLayer(bgtexdata);
        DrawTexLayer(maintexdata);
        DrawTexLayer(foretexdata);
    }

    /*DrawRectangleLines(
        x * TILESIZE * PIXELSCALE * TILESPERCHUNK,
        y * TILESIZE * PIXELSCALE * TILESPERCHUNK,
        TILESPERCHUNK * TILESIZE * PIXELSCALE,
        TILESPERCHUNK * TILESIZE * PIXELSCALE,
        RED
    );*/
}

int Chunk::CheckTileCollision(const Vector2 &tilepos) const {
    int tx = (int)tilepos.x;
    int ty = (int)tilepos.y;

    if (ty < 0 || ty >= (int)colData.size()) return 0;
    if (tx < 0 || tx >= (int)colData[ty].size()) return 0;

    return colData[ty][tx]; // [row][col] = [y][x]
}



void Chunk::Shutdown() {
    //do stuff
    bglayer.RemoveDrawCall(this);
    cout<<"[CHUNK] Shutting down...\n";
    Collision::RemoveCollider(this); //remove from list

    for (Grass* grassobj : grassobjs) {
        delete grassobj;
    }
    grassobjs.clear();
    grassobjs.shrink_to_fit();
}

void Chunk::Startup() {
    //do stuff
    bglayer.AddDrawCall(this, 0);
    Collision::AddCollider(this); //add to list

    for (Vector2& grasspos : grasstileposes) {
        //spawn grass objects
        grassobjs.push_back(new Grass(grasspos));
    }
}

//-----CHUNKMANAGER STUFF-----
ChunkManager::ChunkManager(DrawLayer& _bglayer, EventManager& _playerposmanager) : bglayer(_bglayer), playerposmanager(_playerposmanager) {
    std::cout << "[CHUNK] SplitGrid starting...\n";
    tiletexs = SpriteSplitter::SplitGrid("../Art/Tilemap/Tilemap.png", 8, 8, 4, 5, 1);
    std::cout << "[CHUNK] SplitGrid done. tiletexs.size() = " << tiletexs.size() << "\n";

    int chunksX = 50;
    int chunksY = 2;
    std::cout << "[CHUNK] Resizing coordchunks to " << chunksX << "x" << chunksY << "...\n";

    coordchunks.resize(chunksX, std::vector<Chunk*>(chunksY, nullptr));
    bgtexdata.resize(chunksX, std::vector<std::vector<std::vector<int>>>(chunksY));
    maintexdata.resize(chunksX, std::vector<std::vector<std::vector<int>>>(chunksY));
    foretexdata.resize(chunksX, std::vector<std::vector<std::vector<int>>>(chunksY));
    coldata.resize(chunksX, std::vector<std::vector<std::vector<int>>>(chunksY));

    std::cout << "[CHUNK] Calling ParseData()...\n";
    ParseData();
    for (Chunk* ch : allchunkobjs)
        ch->Shutdown();

    std::cout << "[CHUNK] ParseData() returned.\n";

    playerposmanager.AddSpecialListener("CHUNKMANAGER", this, "PLAYER_POS_UPDATE");
}


ChunkManager::~ChunkManager() {
    for (Texture2D& tex : tiletexs) {
        UnloadTexture(tex);
    }
    for (Chunk* chunkobj : allchunkobjs) {
        delete chunkobj;
    }
}

void ChunkManager::Update() {
    for (Chunk* ch : chunkstokill) {
        ch->Shutdown();
    }
    chunkstokill.clear();
    for (Chunk* chunkobj : activechunkobjs) {
        chunkobj->Update();
    }

}
inline void WorldToTile(const Vector2& worldPos, int& outTileX, int& outTileY)
{
    outTileX = (int)floor(worldPos.x / (TILESIZE * PIXELSCALE));
    outTileY = (int)floor(worldPos.y / (TILESIZE * PIXELSCALE));
}


void ChunkManager::OnSpecialEvent(string &command, vector<string> params) {
    if (command != "PLAYER_POS_UPDATE") return;

    int playerx = stoi(params[0]);
    int playery = stoi(params[1]);

    int cx = playerx / CHUNK_SIZE;
    int cy = playery / CHUNK_SIZE;


    if (GameCamera::chunkloadingState) {
        prevactivechunkobjs = activechunkobjs;
        activechunkobjs.clear();

        // Activate 3x3 around player
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {

                int ncx = cx + dx;
                int ncy = cy + dy;

                // Bounds check
                if (ncx < 0 || ncy < 0) continue;
                if (ncx >= coordchunks.size()) continue;
                if (ncy >= coordchunks[0].size()) continue;

                Chunk* ch = coordchunks[ncx][ncy];
                activechunkobjs.push_back(ch);
            }
        }
    }
    else {
        prevactivechunkobjs = activechunkobjs;
        activechunkobjs = allchunkobjs;
    }


    // Startup new chunks
    for (Chunk* ch : activechunkobjs) {
        if (std::find(prevactivechunkobjs.begin(), prevactivechunkobjs.end(), ch)
            == prevactivechunkobjs.end())
        {
            ch->Startup();
        }
    }

    // Shutdown chunks no longer active
    for (Chunk* ch : prevactivechunkobjs) {
        if (std::find(activechunkobjs.begin(), activechunkobjs.end(), ch)
            == activechunkobjs.end())
        {
            chunkstokill.push_back(ch);
        }
    }



}


void ChunkManager::ParseData() {
    std::ifstream file("../Levels/Level1_parsed.txt");
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open parsed level file.\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }
    file.close();

    // 4 blocks × TILESPERCHUNK lines each
    int linesPerChunk = TILESPERCHUNK * 4;   // 40 lines
    int totalChunks = lines.size() / linesPerChunk;

    std::cout << "File contains " << totalChunks << " chunks\n";

    int chunksX = coordchunks.size();
    int chunksY = coordchunks[0].size();

    if (totalChunks > chunksX * chunksY) {
        std::cout << "ERROR: File contains MORE chunks than grid!\n";
        return;
    }

    int lineIndex = 0;
    int chunkIndex = 0;

    while (chunkIndex < totalChunks) {

        // --- Parse BG layer ---
        std::vector<std::vector<int>> bg(TILESPERCHUNK, std::vector<int>(TILESPERCHUNK));
        for (int r = 0; r < TILESPERCHUNK; r++) {
            std::stringstream ss(lines[lineIndex++]);
            for (int c = 0; c < TILESPERCHUNK; c++) {
                ss >> bg[r][c];
                ss.ignore(1);
            }
        }

        // --- Parse MAIN layer ---
        std::vector<std::vector<int>> main(TILESPERCHUNK, std::vector<int>(TILESPERCHUNK));
        for (int r = 0; r < TILESPERCHUNK; r++) {
            std::stringstream ss(lines[lineIndex++]);
            for (int c = 0; c < TILESPERCHUNK; c++) {
                ss >> main[r][c];
                ss.ignore(1);
            }
        }

        // --- Parse FORE layer ---
        std::vector<std::vector<int>> fore(TILESPERCHUNK, std::vector<int>(TILESPERCHUNK));
        for (int r = 0; r < TILESPERCHUNK; r++) {
            std::stringstream ss(lines[lineIndex++]);
            for (int c = 0; c < TILESPERCHUNK; c++) {
                ss >> fore[r][c];
                ss.ignore(1);
            }
        }

        // --- Parse COLLISION layer ---
        std::vector<std::vector<int>> col(TILESPERCHUNK, std::vector<int>(TILESPERCHUNK));
        for (int r = 0; r < TILESPERCHUNK; r++) {
            std::stringstream ss(lines[lineIndex++]);
            for (int c = 0; c < TILESPERCHUNK; c++) {
                ss >> col[r][c];
                ss.ignore(1);
            }
        }

        // Compute chunk coords
        int cx = chunkIndex % chunksX;
        int cy = chunkIndex / chunksX;

        bgtexdata[cx][cy]   = bg;
        maintexdata[cx][cy] = main;
        foretexdata[cx][cy] = fore;
        coldata[cx][cy]     = col;

        chunkIndex++;
    }

    // Build chunk objects
    for (int cy = 0; cy < chunksY; cy++) {
        for (int cx = 0; cx < chunksX; cx++) {
            if (cx + cy * chunksX >= totalChunks)
                continue;

            Chunk* ch = new Chunk(
                cx, cy,
                bgtexdata[cx][cy],
                maintexdata[cx][cy],
                foretexdata[cx][cy],
                coldata[cx][cy],
                tiletexs,
                bglayer
            );

            coordchunks[cx][cy] = ch;
            allchunkobjs.push_back(ch);
        }
    }

    // ------------------------------------------------------
    // Parse SOUNDS block
    // ------------------------------------------------------
    if (lineIndex < lines.size() && lines[lineIndex] == "SOUNDS") {
        lineIndex++; // move past the SOUNDS header

        while (lineIndex < lines.size()) {
            std::string& sline = lines[lineIndex];
            lineIndex++;

            if (sline.empty()) continue;

            std::stringstream ss(sline);

            std::string id;
            float x, y;
            int radius;
            float minv, maxv;
            std::string path;

            ss >> id >> x >> y >> radius >> minv >> maxv >> path;

            SoundSource src;
            src.pos = {x, y};
            src.soundradius = radius;
            src.minvolume = minv;
            src.maxvolume = maxv;
            src.soundPath = path;
            src.sound = LoadSound(path.c_str());

            SoundSystem::AddSoundSource(src, id);
        }

        std::cout << "Loaded all sound emitters.\n";
    }


    std::cout << "ChunkManager: Level data loaded successfully.\n";
}


int ChunkManager::CheckTileCollision(const Vector2& worldPos)
{
    int tileX = (int)(worldPos.x / (TILESIZE * PIXELSCALE));
    int tileY = (int)(worldPos.y / (TILESIZE * PIXELSCALE));

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    if (cx < 0 || cy < 0) return 0;
    if (cx >= (int)coordchunks.size()) return 0;
    if (cy >= (int)coordchunks[0].size()) return 0;

    int localX = tileX % TILESPERCHUNK;
    int localY = tileY % TILESPERCHUNK;

    if (localX < 0) localX += TILESPERCHUNK;
    if (localY < 0) localY += TILESPERCHUNK;

    int result = coordchunks[cx][cy]->CheckTileCollision({(float)localX, (float)localY});

    if (DEBUG_COLLISION) {
        // draw the tile being sampled
        float wx = (cx * TILESPERCHUNK + localX) * TILESIZE * PIXELSCALE;
        float wy = (cy * TILESPERCHUNK + localY) * TILESIZE * PIXELSCALE;

        DrawRectangleLines(
            (int)wx,
            (int)wy,
            TILESIZE * PIXELSCALE,
            TILESIZE * PIXELSCALE,
            result ? RED : BLUE
        );
    }

    return result;
}

void ChunkManager::ModifyTexData(const Vector2& worldPos, int layer, int tileID)
{
    int tileX, tileY;
    WorldToTile(worldPos, tileX, tileY);

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    if (cx < 0 || cy < 0) return;
    if (cx >= (int)coordchunks.size()) return;
    if (cy >= (int)coordchunks[0].size()) return;

    Chunk* ch = coordchunks[cx][cy];
    if (!ch) return;

    // Only modify if chunk is active
    if (std::find(activechunkobjs.begin(), activechunkobjs.end(), ch) == activechunkobjs.end())
        return;

    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

    switch (layer) {
        case 0: bgtexdata[cx][cy][ly][lx]   = tileID; break;
        case 1: maintexdata[cx][cy][ly][lx] = tileID; break;
        case 2: foretexdata[cx][cy][ly][lx] = tileID; break;
    }
}


void ChunkManager::ModifyCollData(const Vector2& worldPos, int collID)
{
    int tileX, tileY;
    WorldToTile(worldPos, tileX, tileY);

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    if (cx < 0 || cy < 0) return;
    if (cx >= (int)coordchunks.size()) return;
    if (cy >= (int)coordchunks[0].size()) return;

    Chunk* ch = coordchunks[cx][cy];
    if (!ch) return;

    // Only modify if chunk is active
    if (std::find(activechunkobjs.begin(), activechunkobjs.end(), ch) == activechunkobjs.end())
        return;

    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

    coldata[cx][cy][ly][lx] = collID;

    // Rebuild ONLY the chunk's internal hitboxes, not global collision lists
    ch->BuildColliders();
}

int ChunkManager::CheckTileId(const Vector2& worldPos, int layer)
{
    // Convert world → tile
    int tileX, tileY;
    WorldToTile(worldPos, tileX, tileY);

    // Convert tile → chunk
    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    if (cx < 0 || cy < 0) return -1;
    if (cx >= coordchunks.size()) return -1;
    if (cy >= coordchunks[0].size()) return -1;

    // Local tile inside chunk
    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

    // Read from correct layer
    switch (layer) {
        case 0: return bgtexdata[cx][cy][ly][lx];
        case 1: return maintexdata[cx][cy][ly][lx];
        case 2: return foretexdata[cx][cy][ly][lx];
    }

    return -1;
}
void ChunkManager::WorldToTile(const Vector2& worldPos, int& tileX, int& tileY)
{
    // Your world uses 8×8 tiles scaled by 10 → logical tile size = 8
    tileX = (int)floor(worldPos.x / 8.0f);
    tileY = (int)floor(worldPos.y / 8.0f);
}

bool ChunkManager::ValidChunk(int cx, int cy) const {
    if (cx < 0 || cy < 0) return false;
    if (cx >= coordchunks.size()) return false;
    if (cy >= coordchunks[0].size()) return false;
    return true;
}

void ChunkManager::SpawnGrassAtWorld(const Vector2& worldPos)
{
    int tileX, tileY;
    WorldToTile(worldPos, tileX, tileY);

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    if (!ValidChunk(cx, cy))
        return;

    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

    coordchunks[cx][cy]->SpawnGrassAt(lx, ly);
}

bool ChunkManager::TileTouchesAir(const Vector2& worldPos, int layer)
{
    int tileX, tileY;
    WorldToTile(worldPos, tileX, tileY);

    int cx = tileX / TILESPERCHUNK;
    int cy = tileY / TILESPERCHUNK;

    int lx = tileX % TILESPERCHUNK;
    int ly = tileY % TILESPERCHUNK;
    if (lx < 0) lx += TILESPERCHUNK;
    if (ly < 0) ly += TILESPERCHUNK;

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

        if (!ValidChunk(ncx, ncy))
            return true;

        Vector2 wp = {
            (float)(ncx * TILESPERCHUNK + nx) * TILESIZE * PIXELSCALE,
            (float)(ncy * TILESPERCHUNK + ny) * TILESIZE * PIXELSCALE
        };

        int id = CheckTileId(wp, layer);
        if (id == -1)
            return true;
    }

    return false;
}
void ChunkManager::SaveLevel() {
    std::ofstream f("../Levels/Level1_parsed.txt");
    if (!f.is_open()) {
        std::cout << "ERROR: Could not save level.\n";
        return;
    }

    // We must iterate in the exact same order ParseData expects
    // Based on your ParseData, it seems to expect 4 layers per chunk.
    for (int cy = 0; cy < coordchunks[0].size(); cy++) {
        for (int cx = 0; cx < coordchunks.size(); cx++) {

            // 1. BG LAYER (Must write 20 lines)
            for (int r = 0; r < TILESPERCHUNK; r++) {
                for (int c = 0; c < TILESPERCHUNK; c++) {
                    f << bgtexdata[cx][cy][r][c] << (c < TILESPERCHUNK - 1 ? "," : "");
                }
                f << "\n";
            }

            // 2. MAIN LAYER (Must write 20 lines)
            for (int r = 0; r < TILESPERCHUNK; r++) {
                for (int c = 0; c < TILESPERCHUNK; c++) {
                    f << maintexdata[cx][cy][r][c] << (c < TILESPERCHUNK - 1 ? "," : "");
                }
                f << "\n";
            }

            // 3. FORE LAYER (Must write 20 lines)
            for (int r = 0; r < TILESPERCHUNK; r++) {
                for (int c = 0; c < TILESPERCHUNK; c++) {
                    f << foretexdata[cx][cy][r][c] << (c < TILESPERCHUNK - 1 ? "," : "");
                }
                f << "\n";
            }

            // 4. COLLISION LAYER (Must write 20 lines)
            for (int r = 0; r < TILESPERCHUNK; r++) {
                for (int c = 0; c < TILESPERCHUNK; c++) {
                    f << coldata[cx][cy][r][c] << (c < TILESPERCHUNK - 1 ? "," : "");
                }
                f << "\n";
            }
        }
    }

    // Sound emitters section (Make sure ParseData handles this line!)
    f << "SOUNDS\n";

    for (auto& [id, src] : SoundSystem::sources) {
        f << id << " "
          << src.pos.x << " "
          << src.pos.y << " "
          << src.soundradius << " "
          << src.minvolume << " "
          << src.maxvolume << " "
          << src.soundPath << "\n";
    }

    f.close();
    std::cout << "Level saved successfully with all layers." << std::endl;
}
