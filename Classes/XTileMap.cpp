#include "XTileMap.h"
#include "Interactions.h"
#include "SpriteLoader.h"
#include "MyMath.h"
#include "MapGen/MapGenerator.h"
#include "Multiplayer/XClient.h"
#include "Debug.h"

using namespace std;

//#define INFINITE_GENERATION

#define TILE_TO_CHUNK(__TILE_COORDINATES__) Vec2X<int>((int)__TILE_COORDINATES__.x/CHUNK_WIDTH, (int)__TILE_COORDINATES__.y/CHUNK_HEIGHT)

//this is a standard create function of cocos2d-x, I am going to be using these from now on now that I understand how it works
XTileMap* XTileMap::create(std::string mapName)
{ 
	// creates with random seed
	return create(mapName,RandomHelper::random_int(0, 32000));
}

XTileMap* XTileMap::create(std::string mapName, int seed) {
	XTileMap *pRet = new(std::nothrow)XTileMap();
	if (pRet && pRet->init(mapName,seed))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool XTileMap::init(std::string mapName, int seed) {
	if (!Node::init()){
		return false;
	}
	m_mapName = mapName;
	m_seed = seed;
	Vec2i mapSize = MapGenerator::getSize(mapName);

	// checking for valid size
	if (mapSize.x == 0) {
		Debugger::logError("width is 0 of map: " + mapName, DEBUG_MAPGEN);
		return false;
	}
	else if (mapSize.y == 0) {
		Debugger::logError("hieght is 0 of map: " + mapName, DEBUG_MAPGEN);
		return false;
	}

	m_mapSize = mapSize;
	return true;
}

string XTileMap::getTileRegionAt(Vec2i tileCoordinates) {
	return getMapTileAt(tileCoordinates, 0).region;
}

bool XTileMap::isMovable(Vec2i tileCoordinates) {
	if (tileCoordinates.x < 0) return false;
	if (tileCoordinates.y < 0) return false;
	if (tileCoordinates.x >= m_mapSize.x) return false;
	if (tileCoordinates.y >= m_mapSize.y) return false;

	return Interact::isMovable(getTileNameAt(tileCoordinates));
}
void XTileMap::cullChunk(Vec2i chunkCoordinates) {
	if (!isPartOf(renderedChunks, chunkCoordinates)) {
		return;
	}
	// for every change
	for (auto currentChunk: m_generatedMapTiles) {
		if (!currentChunk.chunkCoordinate.equals(chunkCoordinates)) continue; // don't address chunks that the chunk is not relevant to
		
		// for every layer
		for (auto currentLayer : currentChunk.chunkTiles) {
			
			// for every tile in that layer
			for (int x = 0; x < std::min(CHUNK_WIDTH, m_mapSize.x - chunkCoordinates.x * CHUNK_WIDTH); x++) {
				for (int y = 0; y < std::min(CHUNK_HEIGHT, m_mapSize.y - chunkCoordinates.y * CHUNK_HEIGHT); y++) {

					// gets the coordinates of the tile itself, instead of it being relative to the chunk's origin
					Vec2i realCoordinates(x + chunkCoordinates.x * CHUNK_WIDTH, y + chunkCoordinates.y * CHUNK_HEIGHT);

					if (currentLayer.tiles[x][y].name.empty() || getTileState(realCoordinates)== DESTROYED) continue; // If there is no tile there DON'T BLOODY CULL IT!!!!
					
					CCASSERT(currentLayer.tiles[x][y].tile, "Deleting tile that does not exist.");
					currentLayer.tiles[x][y].tile->removeFromParentAndCleanup(true);
				}
			}
		}
		m_generatedMapTiles.remove(currentChunk);
		break;
	}

	renderedChunks.remove(chunkCoordinates);
}

void XTileMap::renderChunk(Vec2i chunkCoordinates) {

	//has this chuck already been rendered?
	if (isPartOf(renderedChunks, chunkCoordinates)) {
		return;
	}
#ifdef INFINITE_GENERATION
	Debugger::logWarning("Infinite map generation enabled", DEBUG_MAPGEN);
	while (true) {
#endif
		//Generate the map for this chunk
		MapInfo generatedMap = MapGenerator::GenerateChunk(m_mapName, chunkCoordinates, m_seed);

		unordered_map<string, Texture2D*> sauceMap;

		//This is done in reverse to satisfy the old right-down render order, it could very well be done the other way but meh
		MapLayer* layers = generatedMap.layers;
		XTileMapChunk chunkSave;
		chunkSave.chunkCoordinate = chunkCoordinates;
		chunkSave.chunkTiles.reserve(LAYER_COUNT);
#define IF_PREGENERATED if(pregenerated)
#define IF_NOT_PREGENERATED if(!pregenerated)
		for (int layerIndex = 0; layerIndex < LAYER_COUNT; layerIndex++) { 
			XTileMapLayer layerSave;
			for (int y = 0; y < std::min(CHUNK_HEIGHT, m_mapSize.y - chunkCoordinates.y * CHUNK_HEIGHT); y++) {

				for (int x = 0; x < std::min(CHUNK_WIDTH, m_mapSize.x - chunkCoordinates.x * CHUNK_WIDTH); x++) {

					// map-relative tile coordinates (in contrast to chunk-relative)
					Vec2i realCoordinates(x + chunkCoordinates.x * CHUNK_WIDTH, y + chunkCoordinates.y * CHUNK_HEIGHT);

					string tileName = layers[layerIndex].tiles[x][y].name;
					if (tileName.empty()) continue; //skip any tiles not on the board

					if (layerIndex == 1 && getTileState(realCoordinates) == DESTROYED) continue; // if the tile has been destroyed

					//Creating the tile sprite
					Node* currentTileSprite = SpriteLoader::loadSprite(tileName,SPRITE_TILE);

					if (currentTileSprite) {

						//Setting anchor point to top corner of the bottom tile, for smooth conversion between world and tile coordinates
						currentTileSprite->setAnchorPoint(Vec2(0.5, (float)1 / ((currentTileSprite->getContentSize().height) / CHUNK_HEIGHT)));

						//Sets the z-order of the sprite so we can move the player through the z order
						setZOrderOfNode(realCoordinates, currentTileSprite, layerIndex);

						//Save the generated information for later
						XMapTile currentTile;
						currentTile.name = tileName;
						currentTile.region = layers[layerIndex].tiles[x][y].region;
						currentTile.tile = currentTileSprite;
						layerSave.tiles[x][y] = currentTile;

						//placement location
						Vec2 worldLocation = findWorldLocationAt(Vec2(realCoordinates.x, realCoordinates.y));
						currentTileSprite->setPosition(worldLocation);
						addChild(currentTileSprite);
					}

					// if the sprite failed to load
					else {
						Debugger::logError("Failed to load tile sprite with name: " + tileName, DEBUG_MAPGEN);
					}
				}
			}
			chunkSave.chunkTiles.push_back(layerSave);
		}
#ifdef INFINITE_GENERATION
	}
#else
	m_generatedMapTiles.push_back(chunkSave);
	renderedChunks.push_back(chunkCoordinates);
#endif
}

Vec2 XTileMap::findTileLocationAt(Vec2 tileCoordinate) {
	tileCoordinate = CC_POINT_POINTS_TO_PIXELS(tileCoordinate);
	Vec2 re;
	re.x = (1.0 / MAPTILE_WIDTH)* tileCoordinate.x - (1.0 / MAPTILE_HEIGHT)*tileCoordinate.y;
	re.y = (-1.0 / MAPTILE_WIDTH)*tileCoordinate.x - (1.0 / MAPTILE_HEIGHT)* tileCoordinate.y;// this is my own Mat2

	return  re;
}

Vec2 XTileMap::findWorldLocationAt(Vec2 tileCoordinate) {
	Vec2 re;
	re.x = (MAPTILE_WIDTH/2) * tileCoordinate.x - (MAPTILE_WIDTH/2) * tileCoordinate.y;
	re.y = -(MAPTILE_HEIGHT/2) * tileCoordinate.x - (MAPTILE_HEIGHT/2) * tileCoordinate.y;
	return CC_POINT_PIXELS_TO_POINTS( re);
}

std::string XTileMap::getTileNameAt(Vec2i tileCoordinates){
	string tileName;
	for (int i = LAYER_COUNT -1; i >= 0; i--) {
		tileName = getMapTileAt(tileCoordinates, i).name;
		int state = getTileState(tileCoordinates);
		if (!tileName.empty() && !(state == DESTROYED && i ==1)) {
			return tileName;
		}
	}
	return "";
}


// This function updates the chunks needed, it got a lot larger as I made it a bit more smart (increasing performance)
void XTileMap::updateChunks(Vec2 tileLocation) {
	Vec2i newChunk = getChunkCoordinates(tileLocation);
	if (renderedChunk != newChunk) {
		Vec2i movement = newChunk - renderedChunk;
		Vec2i firstChunkRenderOffset;
		Vec2i firstChunkCullOffset;
		if (movement.sum() > 0) {
			firstChunkRenderOffset = movement * 2;
			firstChunkCullOffset = movement * 0;
		}
		else if(movement.sum() < 0){
			firstChunkRenderOffset = movement;
			firstChunkCullOffset = movement * -1;
		}
		else {
			firstChunkRenderOffset = movement + (movement.x>0 ? Vec2i(1,0) : Vec2i(0,1));
			firstChunkCullOffset = movement * -1 + (movement.x>0 ? Vec2i(1, 0) : Vec2i(0, 1));
		}

		renderChunk(firstChunkRenderOffset + renderedChunk);
		cullChunk(firstChunkCullOffset + renderedChunk);

		if (movement.componentEquals(0)) {
			Vec2i secondChunkRenderOffset = firstChunkRenderOffset + movement.positive().swap();
			Vec2i secondChunkCullOffset = firstChunkCullOffset + movement.positive().swap();
			
			renderChunk(secondChunkRenderOffset + renderedChunk);
			cullChunk(secondChunkCullOffset + renderedChunk);
		}
		else {
			Vec2i secondChunkRenderOffset = firstChunkRenderOffset + Vec2i(0,(movement*-1).y);
			Vec2i secondChunkCullOffset = firstChunkCullOffset + Vec2i(0, movement.y);
			Vec2i thirdChunkRenderOffset = firstChunkRenderOffset + Vec2i((movement*-1).x,0);
			Vec2i thirdChunkCullOffset = firstChunkCullOffset + Vec2i(movement.x,0);
			
			renderChunk(secondChunkRenderOffset + renderedChunk);
			cullChunk(secondChunkCullOffset + renderedChunk);
			renderChunk(thirdChunkRenderOffset + renderedChunk);
			cullChunk(thirdChunkCullOffset + renderedChunk);
		}

		renderedChunk = newChunk;
	}
}

bool XTileMap::isPartOf(std::list<Vec2i> nodeList, Vec2i vector) {
	if (nodeList.empty()) {
		return false;
	}
	for (list<Vec2i>::iterator tileIterator = nodeList.begin(); tileIterator != nodeList.end(); tileIterator++) {
		if (vector.equals(*tileIterator)) {
			return true;
		}
	}
	return false;
}

Vec2i XTileMap::getMapSize() {
	return m_mapSize;
}

void XTileMap::cullChunkBlock(Vec2i chunkCoordinates) {
	cullChunk(chunkCoordinates);
	cullChunk(chunkCoordinates + Vec2i(0, 1));
	cullChunk(chunkCoordinates + Vec2i(1, 0));
	cullChunk(chunkCoordinates + Vec2i(1, 1));
}

void XTileMap::renderChunkBlock(Vec2i chunkCoordinates) {
	renderedChunk = chunkCoordinates;
	renderChunk(chunkCoordinates + Vec2i(0, 0));
	renderChunk(chunkCoordinates + Vec2i(0, 1));
	renderChunk(chunkCoordinates + Vec2i(1, 0));
	renderChunk(chunkCoordinates + Vec2i(1, 1));
}

// this function is not a valid conversion between the chunk vector space and the tile vector space, rather it finds the chunk that should be rendered to make sure
//that the player does not spot any holes
Vec2i XTileMap::getChunkCoordinates(Vec2 tileCoordinates) {
	return Vec2i((int)(((tileCoordinates.x+0.5) - (CHUNK_WIDTH/2)) / CHUNK_WIDTH), (int)(((tileCoordinates.y+0.5) - (CHUNK_HEIGHT/2)) / CHUNK_HEIGHT)); // This 0,1 is a fix for an issue that I don't get...
}

void XTileMap::setTileState(Vec2i tileCoordinates, int state) {
	// If the current state is currently the state setting
	if (getTileState(tileCoordinates) == state) return;
	// If the state is DESTROYED, destroy the tile
	if ((state == DESTROYED)) {
		if(getMapTileAt(tileCoordinates, 1).tile)
			getMapTileAt(tileCoordinates, 1).tile->removeFromParentAndCleanup(true);
	}
	m_stateTable[tileCoordinates] = state;
	
	XClient::getInstance()->changeMap(tileCoordinates, state);
}

int XTileMap::getTileState(Vec2i tileCoordinates) {
	if (!m_stateTable.count(tileCoordinates)) return UNTOUCHED; //if the tile does not have a registered state, then it will be untouched... I do it this way to make sure another entry is not created for it
	return m_stateTable.at(tileCoordinates);
}

void XTileMap::destroyTile(Vec2i tileCoordinates) {
	setTileState(tileCoordinates, DESTROYED);
}

XMapTile XTileMap::getMapTileAt(Vec2i tileCoordinate, int layerIndex) {
	Vec2i tileChunkCoordinate = TILE_TO_CHUNK(tileCoordinate);
	
	for (list<XTileMapChunk>::iterator currentChunk = m_generatedMapTiles.begin(); currentChunk != m_generatedMapTiles.end(); currentChunk++) {
		if (!currentChunk->chunkCoordinate.equals(tileChunkCoordinate)) continue; // don't address chunks that the tile is not relevant to
		Vec2i specificCoordinates = Vec2i(tileCoordinate.x - currentChunk->chunkCoordinate.x * CHUNK_WIDTH, tileCoordinate.y - currentChunk->chunkCoordinate.y * CHUNK_HEIGHT);
		return currentChunk->chunkTiles.at(layerIndex).tiles[specificCoordinates.x][specificCoordinates.y];
	}

	// Tile has not been rendered, lets do some foresight!
	XMapTile* mapTile = getExternalTile(tileCoordinate);

	return mapTile[layerIndex];
}

void XTileMap::placeNode(Vec2i tileCoordinates, Node* item) {
	item->setPosition(findWorldLocationAt(tileCoordinates));
	addChild(item);
}

void XTileMap::setZOrderOfNode(Vec2 tileCoordinates, Node* item, int layer) {
	// Binary representation of doubles
	unsigned long x = tileCoordinates.x;
	unsigned long y = tileCoordinates.y;
	
	unsigned long mapSizeX = (double)getMapSize().x;
	unsigned long mapSizeY = (double)getMapSize().y;

	item->setLocalZOrder(x+y + (layer*(mapSizeX + mapSizeY)));

}

XMapTile XTileMap::getUpperExternalTile(Vec2i tileCoordinates) {
	XMapTile* mapTiles = getExternalTile(tileCoordinates);
	if(mapTiles[1].name.empty()){
		return mapTiles[0];
	}
	else {
		return mapTiles[1];
	}
}

// Gets the tiles that is not rendered yet
XMapTile* XTileMap::getExternalTile(Vec2i coordinates) {

	XMapTile* tiles = new XMapTile[2];
	MapTile* mapGenTile = MapGenerator::generateTile(m_mapName, coordinates, m_seed);
	
	tiles[0].name = mapGenTile[0].name;
	tiles[0].region = mapGenTile[0].region;
	
	tiles[1].name = mapGenTile[1].name;
	tiles[1].region = mapGenTile[1].region;

	return tiles;
}

std::string XTileMap::getTileProperties(Vec2i tilePosition) {
	if (m_propertiesTable.find(tilePosition) == m_propertiesTable.end())
		return "";
	else
		return m_propertiesTable[tilePosition];
}

void XTileMap::setTileProperies(Vec2i tilePosition, std::string properties) {
	m_propertiesTable[tilePosition] = properties;
}