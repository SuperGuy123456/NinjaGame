#include "Chunks.h"

Chunk::Chunk(int cx, int cy, const vector<vector<int> > &tex, const vector<vector<int> > &col, const vector<Texture2D>& _tiletexs, DrawLayer& _bglayer) : texData(tex), colData(col), tiletexs(_tiletexs), bglayer(_bglayer) {
    x = cx;
    y = cy;
}

Chunk::~Chunk(){}; //no need to unload textures as they are part of chunkmanager

void Chunk::Update() {
    //TODO for everything inside the chunk
}

void Chunk::Draw() {
    int localY = 0;

    if (DEBUG_COLLISION) {
        for (const auto& row : colData) {
            int localX = 0;

            for (int id : row) {
                if (id != 0) {
                    DrawRectangle(
                        x * TILESIZE * PIXELSCALE * TILESPERCHUNK + localX * TILESIZE * PIXELSCALE,
                        y * TILESIZE * PIXELSCALE * TILESPERCHUNK + localY * TILESIZE * PIXELSCALE,
                        TILESIZE * PIXELSCALE,
                        TILESIZE * PIXELSCALE,
                        Color{255, 255, 255, 80}
                    );
                }
                localX++;
            }
            localY++;
        }
    }
    else {
        for (const auto& row : texData) {
            int localX = 0;

            for (int id : row) {

                    DrawTexture(
                tiletexs[id],
                x * TILESIZE*PIXELSCALE*TILESPERCHUNK  + (localX * TILESIZE * PIXELSCALE),
                y * TILESIZE*PIXELSCALE*TILESPERCHUNK + (localY * TILESIZE * PIXELSCALE),
                WHITE
            );

                localX++;
            }
            localY++;
        }
    }

    DrawRectangleLines(
        x * TILESIZE * PIXELSCALE * TILESPERCHUNK,
        y * TILESIZE * PIXELSCALE * TILESPERCHUNK,
        TILESPERCHUNK * TILESIZE * PIXELSCALE,
        TILESPERCHUNK * TILESIZE * PIXELSCALE,
        RED
    );
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
}

void Chunk::Startup() {
    //do stuff
    bglayer.AddDrawCall(this, 0);
}

//-----CHUNKMANAGER STUFF-----
ChunkManager::ChunkManager(DrawLayer& _bglayer, EventManager& _playerposmanager) : bglayer(_bglayer), playerposmanager(_playerposmanager) {
    std::cout << "[CHUNK] SplitGrid starting...\n";
    tiletexs = SpriteSplitter::SplitGrid("../Art/Tilemap/Tilemap.png", 8, 8, 3, 5, 10);
    std::cout << "[CHUNK] SplitGrid done. tiletexs.size() = " << tiletexs.size() << "\n";

    int chunksX = 50;
    int chunksY = 2;
    std::cout << "[CHUNK] Resizing coordchunks to " << chunksX << "x" << chunksY << "...\n";

    coordchunks.resize(chunksX, std::vector<Chunk*>(chunksY, nullptr));
    texdata.resize(chunksX, std::vector<std::vector<std::vector<int>>>(chunksY));
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
    for (Chunk* chunkobj : activechunkobjs) {
        chunkobj->Update();
    }
}

void ChunkManager::OnSpecialEvent(string &command, vector<string> params) {
    if (command != "PLAYER_POS_UPDATE") return;

    int playerx = stoi(params[0]);
    int playery = stoi(params[1]);

    int cx = playerx / CHUNK_SIZE;
    int cy = playery / CHUNK_SIZE;

    std::cout << "Player chunk: cx=" << cx << " cy=" << cy << "\n";


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
            ch->Shutdown();
        }
    }

    cout<<"LENGTH OF ACTIVE CHUNKS: "<<activechunkobjs.size()<<endl;

}


void ChunkManager::ParseData() {
    std::ifstream file("../Levels/Level1_parsed.txt");
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open parsed level file.\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    // Read all non-empty lines
    while (std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }

    file.close();

    // ---------------------------------------------------------
    // Compute total chunks from file
    // ---------------------------------------------------------
    int linesPerChunk = TILESPERCHUNK * 2;   // 20 tex + 20 col = 40
    int totalChunks = lines.size() / linesPerChunk;

    std::cout << "File contains " << totalChunks << " chunks\n";

    // ---------------------------------------------------------
    // Use your existing hardcoded grid (50 x 2)
    // ---------------------------------------------------------
    int chunksX = coordchunks.size();        // 50
    int chunksY = coordchunks[0].size();     // 2

    // Safety check
    if (totalChunks > chunksX * chunksY) {
        std::cout << "ERROR: File contains MORE chunks (" << totalChunks
                  << ") than grid can hold (" << chunksX * chunksY << ")!\n";
        return;
    }

    // ---------------------------------------------------------
    // Parse each chunk
    // ---------------------------------------------------------
    int lineIndex = 0;
    int chunkIndex = 0;

    while (chunkIndex < totalChunks) {

        // --- Parse texture rows ---
        std::vector<std::vector<int>> tex(TILESPERCHUNK, std::vector<int>(TILESPERCHUNK));
        for (int r = 0; r < TILESPERCHUNK; r++) {
            std::stringstream ss(lines[lineIndex++]);
            for (int c = 0; c < TILESPERCHUNK; c++) {
                ss >> tex[r][c];
                ss.ignore(1);
            }
        }

        // --- Parse collision rows ---
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

        std::cout << "PARSE: chunkIndex=" << chunkIndex
                  << " -> cx=" << cx
                  << " cy=" << cy << "\n";

        texdata[cx][cy] = tex;
        coldata[cx][cy] = col;

        chunkIndex++;
    }

    // ---------------------------------------------------------
    // Construct chunk objects
    // ---------------------------------------------------------
    for (int cy = 0; cy < chunksY; cy++) {
        for (int cx = 0; cx < chunksX; cx++) {
            // Only build chunks that exist in file
            if (cx + cy * chunksX >= totalChunks)
                continue;

            Chunk* ch = new Chunk(
                cx, cy,
                texdata[cx][cy],
                coldata[cx][cy],
                tiletexs,
                bglayer
            );

            coordchunks[cx][cy] = ch;
            allchunkobjs.push_back(ch);
        }
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

