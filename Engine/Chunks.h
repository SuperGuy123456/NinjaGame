#ifndef _CHUNKS_H_
#define _CHUNKS_H_

#include "raylib.h"
#include "BaseClasses/Base.h"
#include "Engine/DrawingPipeline.h"
#include "Engine/EventManager.h"
#include "Spritesplitter.h"
#include <fstream>      // ifstream
#include <sstream>      // stringstream
#include <string>       // std::string
#include <vector>       // std::vector
#include <iostream>     // std::cout
#include <cstdlib>      // stoi

//in order to support variable map sizes (although the maps must be divisible CLEANLY into chunks) i must use vectors

//only chunkmanager talks to this
using namespace std;


#define PIXELSCALE 10
#define TILESIZE 8

#define TILESPERCHUNK 10

class Chunk : public Load {
public:
    Chunk(int cx, int cy,
                const vector<vector<int>>& tex,
                const vector<vector<int>>& col,
                const vector<Texture2D>& _tiletexs,
                DrawLayer& _bglayer);

    ~Chunk();

    void Draw() override;
    void Update();

    int CheckTileCollision(const Vector2& tilepos) const; // tilepos is COORDS WITHIN THIS CHUNK

    void Shutdown(); //finsihes up tasks before entering sleep mode (for npc stuff)
    void Startup(); //to tell anything to "wake up"
private:
    int x, y; // chunk coordinates (for drawing)

    const vector<vector<int>>& texData;   // tile IDs
    const vector<vector<int>>& colData;   // collision IDs
    const vector<Texture2D>& tiletexs; //so I don't waste memory and copy textures often

    DrawLayer& bglayer;
};


//everything talks to this
class ChunkManager : public Load, Listener {
public:
    ChunkManager(DrawLayer& _bglayer, EventManager& _playerposmanager);
    ~ChunkManager();

    void Draw() override {};
    void Update(); // calls the chunk updates (only active ones)

    int CheckTileCollision(const Vector2& tilepos) const; //tilepos is GLOBAL COORDS (finds chunk its in, then queries with chunk)

    void OnEvent(string &command) override {}; //dont use this
    void OnSpecialEvent(string &command, vector<string> params) override; //need this for player pos updates
private:

    DrawLayer& bglayer;
    EventManager& playerposmanager;

    const int CHUNK_SIZE = TILESIZE*PIXELSCALE*TILESPERCHUNK;

    vector<Chunk*> allchunkobjs;
    vector<vector<Chunk*>> coordchunks;
    vector<Chunk*> activechunkobjs;
    vector<Chunk*> prevactivechunkobjs; //useful for initializing previously sleeping chunks and telling previosly active ones to shutdown

    //for the chunkdata
    vector<vector<vector<vector<int>>>> texdata; //T-T sorry future me: [chunk x][chunk y][tile x][tile y]
    vector<vector<vector<vector<int>>>> coldata; //same thing >:)

    vector<Texture2D> tiletexs;

    void ParseData(); //parses a GIGA BLOCK of data :|
};
#endif