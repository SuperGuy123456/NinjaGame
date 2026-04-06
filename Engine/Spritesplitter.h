#pragma once
#include "raylib.h"
#include "../BaseClasses/Base.h"
#include <vector>
#include <string>

using namespace std;

class SpriteSplitter
{
public:
	static vector<Texture2D> SplitByHorizontal(const char* path, float slicewidth, float scalefactor); //Loads a image and splits it by a slice width (cookie cutter style)
	static vector<Texture2D> SplitGrid(
	const std::string& path,
	int cellWidth,
	int cellHeight,
	int rows,
	int cols,
	float scale
);
};