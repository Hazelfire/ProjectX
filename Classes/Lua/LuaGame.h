#pragma once
#include "LuaInterpreter.h"

struct LuaCreatureObject {
	int index;
	LuaCreatureObject(int pindex) {
		index = pindex;
	}
	LuaCreatureObject() {
		index = 0;
	}
	operator int() const {
		return index;
	}
};

struct LuaPlayerObject {
	int index;
	LuaPlayerObject(int pindex) {
		index = pindex;
	}
	LuaPlayerObject() {
		index = 0;
	}
	operator int() const {
		return index;
	}
};

struct LuaInventoryObject {
	int index;
	LuaInventoryObject(int pindex) {
		index = pindex;
	}
	LuaInventoryObject() {
		index = 0;
	}
	operator int() const {
		return index;
	}
};

class LuaGame : public LuaInterpreter {
public:
	void callWithPlayer(std::string function, int player);
protected:
	virtual void addFunctions(lua_State*);	
	static void pushTile(lua_State*, Vec2i tilePosition);
	static void pushPlayer(lua_State*, int playerIndex);
	static void pushCreature(lua_State*, int id);

	virtual std::string getScopeName() { return "Game"; };
private:

	// Map Lua function
	static int l_tileConstruct(lua_State*);

	static int l_tileToString(lua_State*);
	static int l_tileGetName(lua_State*);
	static int l_tileGetRegion(lua_State*);
	static int l_tileDestroy(lua_State*);
	static int l_tileSetState(lua_State*);
	static int l_tileGetState(lua_State*);
	static int l_tileGetProperties(lua_State*);
	static int l_tileSetProperties(lua_State*);
	static int l_tileGetPosition(lua_State*);

	// Player Lua functions
	static int l_playerConstruct(lua_State*);
	

	static int l_playerToString(lua_State*);
	static int l_playerGetPosition(lua_State*);
	static int l_playerGetName(lua_State*);
	static int l_playerGetClass(lua_State*);
	static int l_playerGetSpeed(lua_State*);
	static int l_playerSetSpeed(lua_State*);
	static int l_playerGetHealth(lua_State*);
	static int l_playerSetHealth(lua_State*);
	static int l_playerHeal(lua_State*);
	static int l_playerDamage(lua_State*);
	static int l_playerTeleport(lua_State*);
	static int l_playerGiveItem(lua_State*);
	static int l_playerGetItemQuantity(lua_State*);
	static int l_playerTakeItem(lua_State*);
	static int l_playerCanCraftItem(lua_State*);
	static int l_playerHasItemWithTag(lua_State*);

	// Creatures
	static int l_creatureConstruct(lua_State*);

	static int l_creatureToString(lua_State*);
	static int l_creatureMoveTo(lua_State*);
	static int l_creatureMoveOn(lua_State*);
	static int l_creatureGetPosition(lua_State*);
	static int l_creatureGetSpeed(lua_State*);
	static int l_creatureSetSpeed(lua_State*);
	static int l_creatureGetName(lua_State*);
	static int l_creatureGetAI(lua_State*);
	static int l_creatureHasTag(lua_State*);
	static int l_creatureGetProperties(lua_State*);
	static int l_creatureSetProperties(lua_State*);
	static int l_creatureDespawn(lua_State*);

	// Music
	static int l_playSong(lua_State*);
	
	// Particles
	static int l_spawnParticles(lua_State*);

	static int l_print(lua_State*);
	static int l_clear(lua_State*);
	static int l_help(lua_State*);
};