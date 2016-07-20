#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <list>
#include <string>

#define RULE_TYPE 0
#define TILE_TYPE 1
#define SCHEM_TYPE 2
#define BLANK_TYPE 3
#define AREA_TYPE 4

struct RuleTile {
	std::string ruleType;
	std::string name;
	char ruleTileType;
	float scale;
	std::string inside;
	double height;
	double supressBase = 3.0;
	double scaleBase = 3.0;
	int iterations = 10;
	int ruleTilesSize = 0;
	RuleTile* ruleTiles;
	std::string region;
};

struct TerrainLayer {
	std::string name;
	int rulesSize = 0;
	RuleTile* rules;
};

struct MapPreset {
	std::string name;
	int width;
	int height;
	int layersSize = 0;
	TerrainLayer* layers;

};
struct TerrainData {
	int mapsSize = 0;
	MapPreset* maps;
};

class TerrainParser {
public:
	static TerrainData parse(std::string source);

private:
	struct Impl;
	static Impl impl;
};
#endif