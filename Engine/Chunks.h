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
#include "Engine/Collision.h"
#include "World/Grass.h"
#include "SoundSystem.h"
class SoundSystem;
//in order to support variable map sizes (although the maps must be divisible CLEANLY into chunks) i must use vectors

//only chunkmanager talks to this
using namespace std;


#define PIXELSCALE 1
#define COLLIDERSCALE 1
#define TILESIZE 8

#define TILESPERCHUNK 10

class Chunk : public Load, public HasCollider {
public:
    Chunk(int cx, int cy,const vector<vector<int>>& _bgtexdata, const vector<vector<int> >& _maintexdata, const vector<vector<int>>& _foretexdata, const vector<vector<int> > &col, const vector<Texture2D>& _tiletexs, DrawLayer& _bglayer);

    ~Chunk();

    void Draw() override;
    void Update();

    int CheckTileCollision(const Vector2& tilepos) const; // tilepos is COORDS WITHIN THIS CHUNK

    void Shutdown(); //finsihes up tasks before entering sleep mode (for npc stuff)
    void Startup(); //to tell anything to "wake up"

    void BuildColliders();
    void SpawnGrassAt(int lx, int ly);


    vector<Grass*> grassobjs;
private:
    int x, y; // chunk coordinates (for drawing)
    const vector<vector<int>>& bgtexdata;
    const vector<vector<int>>& maintexdata;   // tile IDs for main chunk layer
    const vector<vector<int>>& foretexdata;
    const vector<vector<int>>& colData;   // collision IDs
    const vector<Texture2D>& tiletexs; //so I don't waste memory and copy textures often
    vector<Vector2> grasstileposes;

    DrawLayer& bglayer;
    void DrawTexLayer(const std::vector<std::vector<int>>& layer);
};


//everything talks to this
class ChunkManager : public Load, Listener {
public:
    ChunkManager(DrawLayer& _bglayer, EventManager& _playerposmanager);
    ~ChunkManager();

    void Draw() override {};
    void Update(); // calls the chunk updates (only active ones)

    int CheckTileCollision(const Vector2& worldPos); //tilepos is GLOBAL COORDS (finds chunk its in, then queries with chunk)
    int CheckTileId(const Vector2& worldPos, int layer);


    void OnEvent(string &command) override {}; //dont use this
    void OnSpecialEvent(string &command, vector<string> params) override; //need this for player pos updates

    void ModifyTexData(const Vector2& worldPos, int layer, int tileID);
    void ModifyCollData(const Vector2& worldPos, int collID);
    void SpawnGrassAtWorld(const Vector2& worldPos);


    void WorldToTile(const Vector2& worldPos, int& tileX, int& tileY);

    bool ValidChunk(int cx, int cy) const;

    bool TileTouchesAir(const Vector2& worldPos, int layer);

    void SaveLevel();
private:
    DrawLayer& bglayer;
    EventManager& playerposmanager;

    const int CHUNK_SIZE = TILESIZE*PIXELSCALE*TILESPERCHUNK;

    vector<Chunk*> allchunkobjs;
    vector<vector<Chunk*>> coordchunks;
    vector<Chunk*> activechunkobjs;
    vector<Chunk*> prevactivechunkobjs; //useful for initializing previously sleeping chunks and telling previosly active ones to shutdown
    vector<Chunk*> chunkstokill;

    //for the chunkdata
    vector<vector<vector<vector<int>>>> bgtexdata;
    vector<vector<vector<vector<int>>>> maintexdata; //T-T sorry future me: [chunk x][chunk y][tile x][tile y]
    vector<vector<vector<vector<int>>>> foretexdata;
    vector<vector<vector<vector<int>>>> coldata; //same thing >:)

    vector<Texture2D> tiletexs;

    void ParseData(); //parses a GIGA BLOCK of data :|
};
#endif