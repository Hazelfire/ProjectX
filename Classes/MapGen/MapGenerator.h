#pragma once
#include <unordered_map>
#include "MyMath.h"
#include "../SizeMacros.h"
USING_NS_CC;

#define LAYER_COUNT 2

using namespace std;

struct MapTile {
	string name;
	string region;
};

struct MapLayer {
	string name;
	MapTile tiles[CHUNK_WIDTH][CHUNK_HEIGHT];
};

struct MapInfo {
	Vec2i mapSize;
	MapLayer layers[LAYER_COUNT];
};

class MapGenerator {
public:

	//Gets the size of the map specified
	static Vec2i getSize(string);

	//Generates the chunk based on a seed, coordinates and mapName
	static MapInfo GenerateChunk(string mapName, Vec2i chunkCoordinates, int seed);
	static MapTile* generateTile(string mapName, Vec2i tileCoordinates, int seed);

	//Returns a layer count
	static int countLayers(string, string);
private:
	class Impl;
	static Impl impl;
	
};