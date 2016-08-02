#pragma once

#include "2d/CCNode.h"
#include <unordered_map>
#include "SizeMacros.h"
#include "MyMath.h"
USING_NS_CC;

#define UNTOUCHED 0
#define DESTROYED 1
#define DEPLETED 2

struct XMapTile {
	std::string name;
	std::string region;
	Node* tile = nullptr;
};

struct XTileMapLayer {
	XMapTile tiles[CHUNK_WIDTH][CHUNK_HEIGHT];
};

struct XTileMapChunk {
	Vec2i chunkCoordinate;
	std::vector<XTileMapLayer> chunkTiles;
	bool operator==(XTileMapChunk other) { return (chunkCoordinate == other.chunkCoordinate); };
};

class XTileMap : public cocos2d::Node {
	public:

	// Creates the map based off its name
	static XTileMap* create(std::string mapName);

	// Creates a map based off a map name and seed
	static XTileMap* create(std::string mapName, int seed);

	// initializes the map
	virtual bool init(std::string mapName, int seed);

	// gets the region of the tile at the specified coordinates
	std::string getTileRegionAt(Vec2i);

	// returns if you can move on a certain tile
	bool isMovable(Vec2i);

	static Vec2 findTileLocationAt(Vec2 tileCoordinate);
	static Vec2 findWorldLocationAt(Vec2 tileCoordinate);

	// This returns the name of the upmost tile
	std::string getTileNameAt(Vec2i); 
	std::string getTileNameAt(Vec2i, int layerIndex);


	void updateChunks(Vec2);
	void cullChunkBlock(Vec2i);
	void renderChunkBlock(Vec2i);

	Vec2i getChunkCoordinates(Vec2);
	Vec2i getMapSize();

	void setTileState(Vec2i, int);
	int getTileState(Vec2i);

	void destroyTile(Vec2i);

	XMapTile getMapTileAt(Vec2i, int);

	void placeNode(Vec2i tileCoordinates,Node* item);

	void setZOrderOfNode(Vec2 tileCoordinates, Node* item, int layer);

	std::string getTileProperties(Vec2i tilePosition);
	void setTileProperies(Vec2i tilePosition, std::string properties);
private:
	std::list<XTileMapChunk> m_generatedMapTiles;
	std::string m_terrainFile;
	std::string m_mapName;

	// Gets the highest tile not rendered
	XMapTile getUpperExternalTile(Vec2i tileCoordinates);

	// Gets a tile that is not rendered yet
	XMapTile* getExternalTile(Vec2i coordinates);

	std::unordered_map<Vec2i, int> m_stateTable;
	std::unordered_map<Vec2i, std::string> m_propertiesTable;

	bool isPartOf(std::list<Vec2i> nodeList, Vec2i vector);
	std::list<Vec2i> renderedChunks;
	Vec2i renderedChunk;
	void cullChunk(Vec2i);
	void renderChunk(Vec2i);
	int m_seed;
	Vec2i m_mapSize;
};
