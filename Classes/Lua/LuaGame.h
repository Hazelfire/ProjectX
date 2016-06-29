#pragma once
#include "LuaInterpreter.h"

typedef int LuaCreatureObject;
typedef int LuaPlayerObject;
typedef int LuaInventoryObject;

class LuaGame : public LuaInterpreter {
public:
	void callWithPlayer(std::string function, int player);
protected:
	virtual void addFunctions(lua_State*);
private:

	// Map Lua functions
	static int l_findNameAt(lua_State*);
	static int l_findRegionAt(lua_State*);
	static int l_interactMap(lua_State*);
	static int l_destroyTile(lua_State*);
	static int l_setTileState(lua_State*);
	static int l_getTileState(lua_State*);
	static int l_getTileProperties(lua_State*);
	static int l_setTileProperties(lua_State*);

	// Player Lua functions
	static int l_getPlayerByIndex(lua_State*);

	static int l_getPlayerPosition(lua_State*);
	static int l_getPlayerRealPosition(lua_State*);
	static int l_getPlayerName(lua_State*);
	static int l_getClassName(lua_State*);
	static int l_getPlayerMovementSpeed(lua_State*);
	static int l_getPlayerInventory(lua_State*);

	// Inventory
	static int l_givePlayerItem(lua_State*);
	static int l_getQuantityOf(lua_State*);
	static int l_takeItem(lua_State*);
	static int l_canCraftItem(lua_State*);
	static int l_hasItemWithTag(lua_State*);

	// Creatures
	static int l_spawnCreature(lua_State*);

	static int l_creatureMoveTo(lua_State*);
	static int l_creatureMoveOn(lua_State*);
	static int l_getCreaturePosition(lua_State*);
	static int l_getCreatureRealPosition(lua_State*);
	static int l_setCreatureMovementSpeed(lua_State*);
	static int l_getCreatureMovementSpeed(lua_State*);
	static int l_getCreatureName(lua_State*);
	static int l_getCreatureAI(lua_State*);
	static int l_creatureHasTag(lua_State*);
	static int l_getCreatureProperties(lua_State*);
	static int l_setCreatureProperties(lua_State*);

	// Music
	static int l_playSong(lua_State*);
	static int l_playSongRegion(lua_State*);
	
	// Particles
	static int l_spawnParticles(lua_State*);

};