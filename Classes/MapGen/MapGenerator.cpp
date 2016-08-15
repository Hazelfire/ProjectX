#include "MapGenerator.h"
#include "PerlinNoise.h"
#include <unordered_map>
#include "ScriptLoader.h"
#include "Parser/SchematicsParser.h"
#include "Parser/Terrain.h"

class MapGenerator::Impl {
public:
	static MapTile applyRule(int x, int y, const RuleTile& rule, int& seed, string region, int layerIndex);
	static MapTile applyNode(RuleTile tile, std::string region, int x, int y, int& seed, int layerIndex);
	// Flags the map for a schematic, to make sure that it is placed
	static bool flagMap(std::string schematic, Vec2i location, std::string& firstTileName);
	static const MapPreset* findMapWithName(const TerrainData&, string);
	static unordered_map<Vec2i, std::string> firstLayerRegions;
	static unordered_map<Vec2i, std::string> m_flagMap[LAYER_COUNT];
	static TerrainData m_terrainData;
};

MapGenerator::Impl MapGenerator::impl;

unordered_map<Vec2i, std::string> MapGenerator::Impl::firstLayerRegions;
unordered_map<int, PerlinNoise> pNoiseEngines;
TerrainData MapGenerator::Impl::m_terrainData;
unordered_map<Vec2i, std::string> MapGenerator::Impl::m_flagMap[LAYER_COUNT];

#define LAZY_LOAD_MAP_DATA() if(impl.m_terrainData.mapsSize ==0) \
					impl.m_terrainData = TerrainParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_MAP))

//Based off of the Generate function, generates MapInfo in the same way, just for the tile co-ordinates of a single chunk
MapInfo MapGenerator::GenerateChunk(string mapName, Vec2i chunkCoordinates, int seed) {
	MapInfo re;
	//For readability
	Vec2i corner = Vec2i(chunkCoordinates.x * CHUNK_WIDTH, chunkCoordinates.y * CHUNK_HEIGHT);

	//Getting information from the terrain file (lazy loading)
	LAZY_LOAD_MAP_DATA();
	const MapPreset* map = impl.findMapWithName(impl.m_terrainData, mapName);

	//setting width and height
	re.mapSize.x = map->width;
	re.mapSize.y = map->height;


	//input Layers
	const TerrainLayer* terrainLayers = map->layers;
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			Vec2i tileCoordinates = Vec2i(x, y) + corner;
			MapTile* currentTiles= generateTile(mapName,tileCoordinates, seed);
			re.layers[0].tiles[x][y] = currentTiles[0];
			if(!currentTiles[1].name.empty())
				re.layers[1].tiles[x][y] = currentTiles[1];
			delete[] currentTiles;
		}
	}
	return re;
}

const MapPreset* MapGenerator::Impl::findMapWithName(const TerrainData& terrainData, string mapName) {
	for (int i = 0;i < terrainData.mapsSize; i++) {
		if (terrainData.maps[i].name == mapName) {
			return &terrainData.maps[i];
		}
	}
	return nullptr;
}

MapTile* MapGenerator::generateTile(string mapName, Vec2i tileCoordinates, int seed) {
	LAZY_LOAD_MAP_DATA();
	const MapPreset* map = impl.findMapWithName(impl.m_terrainData, mapName);
	int metaseed = seed;
	srand(hash<int>()(seed + tileCoordinates.expand(map->width)));
	MapTile* tiles = new MapTile[LAYER_COUNT];
	for (int layerIndex = 0; layerIndex < map->layersSize; layerIndex++) {
		for (int i = 0; i < map->layers[layerIndex].rulesSize; i++) {
			//Applies the rule
			MapTile currentTile = impl.applyRule(tileCoordinates.x, tileCoordinates.y, map->layers[layerIndex].rules[i], seed, "Map", layerIndex);
			if (!currentTile.name.empty()) {
				tiles[layerIndex] = currentTile;
			}
		}
		if (layerIndex == 0)
			impl.firstLayerRegions[tileCoordinates] = tiles[layerIndex].region;
	}
	return tiles;
}

float myRand(unsigned int seed) {
	return (float)((seed * 52253) & 65535) / 65535;
}

MapTile MapGenerator::Impl::applyRule(int x, int y,const RuleTile& rule, int& seed, string ruleRegion, int layerIndex){
	MapTile re;
	double randomHeight = 100;
	bool inside = false;

	// If the tile has already been flagged AND it is the second layer
	if (m_flagMap[layerIndex].find(Vec2i(x, y)) != m_flagMap[layerIndex].end()) {
		re.name = m_flagMap[layerIndex][Vec2i(x, y)];
		return re;
	}

	if (rule.ruleType== "perlin") {
		//If there has not already been an engine generated for this seed
		if(pNoiseEngines.find(seed) == pNoiseEngines.end()){
			//Create the engine
			PerlinNoise pn(seed);
			pNoiseEngines[seed] = pn;

			randomHeight = pn.noise((double)x*(1.0/rule.scale) / 50, (double)y*(1.0/rule.scale) / 50);
		}
		else {
			PerlinNoise pn = pNoiseEngines[seed];
			randomHeight = pn.noise((double)x*(1.0/rule.scale) / 50, (double)y*(1.0/rule.scale) / 50);
		}
	}

	else if (rule.ruleType== "random") {
		randomHeight = myRand(seed ^ Vec2i(x,y).hash()) * 100;
	}
	else if (rule.ruleType== "inside") {
		if (firstLayerRegions[Vec2i(x,y)] == rule.inside) {
			inside = true;
		}
		else {
			return re;
		}
	}
	else if (rule.ruleType == "fractal") {
		double scaleBase = rule.scaleBase;
		double suppresionBase = rule.supressBase;
		int fractalIterations = rule.iterations;
		if(pNoiseEngines.find(seed) == pNoiseEngines.end()){
		//Create the engine
			PerlinNoise pn(seed);
			pNoiseEngines[seed] = pn;
			double heightCount = 0.0;
			for (int i = 0; i < fractalIterations; i++) {
				if (i % 4 == 0) {
					heightCount += (pn.noise((double)(x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)y*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 1) {
					heightCount += (pn.noise((double)(32000 - x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)y*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 2) {
					heightCount += (pn.noise((double)x*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)(32000 - y)*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 3) {
					heightCount += (pn.noise((double)(32000 - x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)(32000 - y)*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
			}

			double divisor = 0.0;
			for (int i = 0; i < fractalIterations; i++)
				divisor += 1.0 / powf(suppresionBase, i);

			randomHeight = heightCount / divisor;
		}
		else {
			PerlinNoise pn = pNoiseEngines[seed];
			double heightCount = 0.0;
			for (int i = 0; i < fractalIterations; i++) {
				if (i % 4 == 1) {
					heightCount += (pn.noise((double)(x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)y*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 2) {
					heightCount += (pn.noise((double)(32000 - x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)y*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 3) {
					heightCount += (pn.noise((double)x*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)(32000 - y)*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
				else if (i % 4 == 0) {
					heightCount += (pn.noise((double)(32000 - x)*((1.0/rule.scale) * (powf(scaleBase, i))) / 50, (double)(32000 - y)*((1.0/rule.scale) * powf(scaleBase, i)) / 50)) / (powf(suppresionBase, i));
				}
			}
			double divisor = 0.0;
			for (int i = 0; i < fractalIterations; i++)
				divisor += 1.0 / powf(suppresionBase, i);
			randomHeight = heightCount / divisor;
		}
	}

	RuleTile* subRules = rule.ruleTiles;
	for (int i = 0; i < rule.ruleTilesSize; i++) {
		RuleTile currentSubrule = subRules[i];
		if (currentSubrule.height > randomHeight || inside) {			
			return applyNode(currentSubrule, ruleRegion, x, y, seed, layerIndex);
			
			
		}
	}
	return re;
}

MapTile MapGenerator::Impl::applyNode(RuleTile tile, std::string ruleRegion, int x, int y, int& seed, int layerIndex) {
	seed++;
	string subRuleRegion = tile.region;
	string region = subRuleRegion.empty() ? ruleRegion : subRuleRegion;
	
	// If the child is a tile
	if (tile.ruleTileType == TILE_TYPE) {
		MapTile currentMapTile;
		currentMapTile.name = tile.name;
		currentMapTile.region = region;
		//CCLOG(region.c_str());
		return currentMapTile;
	}

	// If the child is a rule
	else if (tile.ruleTileType == RULE_TYPE) {
		return applyRule(x, y, tile, seed, region, layerIndex);
	}

	// If the child is a schematic
	else if (tile.ruleTileType == SCHEM_TYPE) {
		std::string tileName;
		flagMap(tile.name, Vec2i(x, y), tileName);

		MapTile currentMapTile;
		currentMapTile.name = tileName;
		currentMapTile.region = region;
		return currentMapTile;

	}

	// If the child is a blank
	else if (tile.ruleTileType == BLANK_TYPE) {
		MapTile blankTile;
		return blankTile;
	}

	// If the child is an area
	else if (tile.ruleTileType == AREA_TYPE) {
		MapTile currentMapTile;
		for (int subRuleIndex = 0; subRuleIndex < tile.ruleTilesSize; subRuleIndex++) {
			MapTile tileCandidate = applyNode(tile.ruleTiles[subRuleIndex], region, x, y, seed, layerIndex);
			if (!tileCandidate.name.empty())
				currentMapTile = tileCandidate;
		}
		return currentMapTile;
	}
}

Vec2i MapGenerator::getSize(string mapName) {
	LAZY_LOAD_MAP_DATA();
	const MapPreset* map = impl.findMapWithName(impl.m_terrainData, mapName);

	return Vec2i(map->width, map->height);
}

int MapGenerator::countLayers(string filename, string mapName) {
	LAZY_LOAD_MAP_DATA();
	return impl.findMapWithName(impl.m_terrainData, mapName)->layersSize;
}

bool MapGenerator::Impl::flagMap(std::string schematic, Vec2i location, std::string& firstTileName) {
	SchematicMap schematics = SchematicsParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SCHEMATICS));
	Schematic schematicToPlace = schematics.at(schematic);
	for (int layerIndex = 0; layerIndex < LAYER_COUNT; layerIndex++) {
		for (int x = 0; x < schematicToPlace.width; x++) {
			for (int y = 0; y < schematicToPlace.height; y++) {
				Vec2i tileLocation = Vec2i(x, y) + location;

				// If the tile has already been flagged by another structure
				if (m_flagMap[layerIndex].find(tileLocation) != m_flagMap[layerIndex].end()) {
					return false;
				}
			}
		}
	}

	// Now that we have checked that we can place the schematic, lets place it!
	for (int layerIndex = 0; layerIndex < LAYER_COUNT; layerIndex++) {
		for (int x = 0; x < schematicToPlace.width; x++) {
			for (int y = 0; y < schematicToPlace.height; y++) {
				if (x == 0 && y == 0) {
					firstTileName = schematicToPlace.data[layerIndex][x][y];
				}
				Vec2i tileLocation = Vec2i(x, y) + location;
				if (!schematicToPlace.data[layerIndex][x][y].empty()) {
					m_flagMap[layerIndex][tileLocation] = schematicToPlace.data[layerIndex][x][y];
				}
			}
		}
	}

	// Sucess!
	return true;
}
