#include "LuaGame.h"
#include "Interactions.h"
#include "Arena.h"
#include "Inventory.h"
#include "Creatures.h"
#include "MusicEngine.h"
#include "Multiplayer/PuppetMaster.h"
#include "Multiplayer/XClient.h"
#include <sstream>
#include "Particles.h"
#include "Mechs/Health.h"
#include "MyUI/LuaTerminal.h"


void LuaGame::addFunctions(lua_State* mainState) {
	LuaInterpreter::addFunctions(mainState);
	
	pushCFunction(mainState,
		0,
		{ "playSong",
		"plays the song given",
		{ { { LUA_TSTRING, "song" } } },
		0,
		l_playSong });
	lua_setglobal(mainState, "playSong");

	// Particles
	pushCFunction(mainState,
		0,
		{ "spawnParticles",
		"Spawns the particle system at the given location",
		{ { { LUA_TSTRING,  "particlSystem" }, { LUA_TVECTOR, "position" } } },
		0,
		l_spawnParticles });
	lua_setglobal(mainState, "spawnParticles");

	// Terminal
	pushCFunction(mainState,
		0,
		{ "print",
		"prints a message to the console, and will print anything",
		{ { { -1, "" }} }, // -1 is a magic number, basically means that it takes any number of arguments
		0,
		l_print });
	lua_setglobal(mainState, "print");

	pushCFunction(mainState,
		0,
		{ "clear",
		"clears the terminal log",
		{ {} },
		0,
		l_clear });
	lua_setglobal(mainState, "clear");

	lua_pushinteger(mainState, UNTOUCHED);
	lua_setglobal(mainState, "UNTOUCHED");

	lua_pushinteger(mainState, DESTROYED);
	lua_setglobal(mainState, "DESTROYED");

	lua_pushinteger(mainState, DEPLETED);
	lua_setglobal(mainState, "DEPLETED");
	
	pushCFunction(mainState, 0,
	{ "creature",
	"Spawns a creature at a position and returns an instance of it",
	{ { { LUA_TSTRING, "creatureName" }, { LUA_TVECTOR, "spawnPosition" } } },
	LUA_TCREATURE,
	l_creatureConstruct });

	lua_setglobal(mainState, "creature");

	pushCFunction(mainState,
		0,
		{ "tile",
		"Returns the tile at a given position",
		{ { {LUA_TNUMBER, "x"}, {LUA_TNUMBER, "y"} },
		{ { LUA_TVECTOR, "tilePosition" } }},
		LUA_TTILE,
		l_tileConstruct });

	lua_setglobal(mainState, "tile");
}

void LuaGame::callWithPlayer(std::string function, int player) {
	lua_getglobal(m_mainState, function.c_str());

	pushPlayer(m_mainState, player);

	int error = lua_pcall(m_mainState,1, 0, 0);
	if (error) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		CCLOG(lua_tostring(m_mainState, -1));
#endif
		lua_pop(m_mainState, 1);
	}
}

// tile

void LuaGame::pushTile(lua_State* state, Vec2i tilePosition) {
	pushObject(state, "tile", {
		{ "getName",
		"Returns the name of the tile",
		{ { } },
		LUA_TSTRING,
		l_tileGetName },

		{ "getRegion",
		"Returns the region the tile is inside",
		{ { } },
		LUA_TSTRING,
		l_tileGetRegion},

		{ "destroy",
		"Destroys the tile",
		{ { } },
		0,
		l_tileDestroy},

		{ "setState",
		"Sets the state of the tile",
		{ { { LUA_TNUMBER, "state" } } },
		0,
		l_tileSetState},

		{ "getState",
		"Returns the state of the tile",
		{ { } },
		LUA_TNUMBER,
		l_tileGetState},

		{ "setProperties",
		"Sets the property table associated with the tile",
		{ { { LUA_TTABLE, "propertiesTable" } } },
		0,
		l_tileSetProperties},

		{ "getProperties",
		"Returns the property table associated with the tile",
		{ { } },
		LUA_TTABLE,
		l_tileGetProperties},

		{ "getPosition",
		"Returns the position vector of the tile",
		{ { } },
		LUA_TVECTOR,
		l_tileGetPosition}
	});

	lua_getmetatable(state, -1);
	
	lua_pushstring(state, "position");
	pushVector(state, tilePosition);
	lua_settable(state, -3);

	lua_setmetatable(state, -2);
}

int LuaGame::l_tileConstruct(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		pushTile(functionState, Vec2i(x, y));
	}
	else if (lua_gettop(functionState) == 1) {
		pushTile(functionState, toVector(functionState, 1));
	}

	return 1;
}

int LuaGame::l_tileGetName(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	string name = Arena::getMapInstance()->getTileNameAt(position);
	lua_pushstring(functionState, name.c_str());
	
	return 1;
}

int LuaGame::l_tileGetRegion(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	string name = Arena::getMapInstance()->getTileRegionAt(position);
	lua_pushstring(functionState, name.c_str());
	return 1;
}

int LuaGame::l_tileDestroy(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	Arena::getMapInstance()->destroyTile(position);
	return 0;
}

int LuaGame::l_tileSetState(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	int state = lua_tointeger(functionState, 1);
	Arena::getMapInstance()->setTileState(position, state);
	

	return 0;
}

int LuaGame::l_tileGetState(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	int state = Arena::getMapInstance()->getTileState(position);
	
	lua_pushinteger(functionState, state);
	return 1;
}

int LuaGame::l_tileSetProperties(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	std::string pickledTable = pickleTable(functionState, 1);
					
	// send everyone the changes and also change our own map
	XClient::getInstance()->setTileProperties(position, pickledTable);
	Arena::getMapInstance()->setTileProperies(position, pickledTable);
	return 0;
}

int LuaGame::l_tileGetProperties(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_VEC(position, "position");

	std::string pickledTable = Arena::getMapInstance()->getTileProperties(position);
	if(pickledTable.empty())
		lua_newtable(functionState);
	else
		unPickleTable(functionState, pickledTable);
	return 1;
}

int LuaGame::l_tileGetPosition(lua_State* functionState) {
	CHECK_ARGS;

	pushSelfMeta(functionState, "position");
	return 1;
}

		/*
	static int l_playerToString(lua_State*);
	static int l_playerGetPosition(lua_State*);
	static int l_playerGetName(lua_State*);
	static int l_playerGetClass(lua_State*);
	static int l_playerGetSpeed(lua_State*);
	}
	return 0;
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
	*/
		
void LuaGame::pushPlayer(lua_State* state, int playerIndex) {
	pushObject(state,"player", {
		{ "toString",
		"Returns a string representation of the player",
		{ { } },
		LUA_TSTRING,
		l_playerToString},

		{ "getPosition",
		"Returns the position vector of the player in tile coordiantes",
		{ { } },
		LUA_TVECTOR,
		l_playerGetPosition},

		{ "getName",
		"Returns the chosen name of the player",
		{ { } },
		LUA_TSTRING,
		l_playerGetName},

		{ "getClass",
		"Returns the chosen class of the player",
		{ { } },
		LUA_TSTRING,
		l_playerGetClass},

		{ "getSpeed",
		"Return the movement speed of the player in tiles per second",
		{ { } },
		LUA_TNUMBER,
		l_playerGetSpeed},

		{ "setSpeed",
		"Sets the movement speed of the player in tiles per second",
		{ { { LUA_TNUMBER, "speed" } } },
		0,
		l_playerSetSpeed},

		{ "getHealth",
		"Returns the health of the player, out of 100",
		{ { } },
		LUA_TNUMBER,
		l_playerGetHealth},

		{ "setHealth",
		"Sets the health of the player, out of 100",
		{ { { LUA_TNUMBER, "health" } } },
		0,
		l_playerSetHealth},

		{ "heal",
		"Heals the player by the given HP",
		{ { { LUA_TNUMBER, "hp" } } },
		0,
		l_playerHeal},

		{ "damage",
		"Damages the player by the given HP",
		{ { { LUA_TNUMBER, "hp" } } },
		0,
		l_playerDamage},

		{ "teleport",
		"Teleports the player to the given location",
		{ { { LUA_TVECTOR, "position" } } },
		0,
		l_playerTeleport},

		{ "giveItem",
		"Gives the item to the player, in the quantity if given, gives one if not",
		{ { { LUA_TSTRING, "itemName" } },
		  { { LUA_TSTRING, "itemName" }, { LUA_TNUMBER, "quantity" } } },
		0,
		l_playerGiveItem},

		{ "takeItem",
		"Takes the item from the player, in the quantity of give, takes one if not",
		{ { { LUA_TSTRING, "itemName" } },
		  { { LUA_TSTRING, "itemName" }, { LUA_TNUMBER, "quantity" } } },
		0,
		l_playerTakeItem},

		{ "getItemQuantity",
		"Returns the quantity owned of the item given",
		{ { { LUA_TSTRING, "itemName" } } },
		LUA_TNUMBER,
		l_playerGetItemQuantity},

		{ "canCraftItem",
		"Returns if the player can craft a certain item",
		{ { { LUA_TSTRING, "itemName" } } },
		LUA_TBOOLEAN,
		l_playerCanCraftItem},

		{ "hasItemWithTag",
		"Returns if the player has an item with a given tag",
		{ { { LUA_TSTRING, "tag" } } },
		LUA_TBOOLEAN,
		l_playerHasItemWithTag}
	
	});

	lua_getmetatable(state, -1);
	
	lua_pushstring(state, "index");
	lua_pushinteger(state, playerIndex);
	lua_settable(state, -3);

	lua_setmetatable(state, -2);
}

int LuaGame::l_playerConstruct(lua_State* functionState) {
	CHECK_ARGS;

	int playerIndex = lua_tointeger(functionState, 1);
	pushPlayer(functionState, playerIndex);

	return 1;
}

int LuaGame::l_playerToString(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	Mortal* player = PuppetMaster::getPlayer(index);
	
	std::string stringRepresentation = "Player " +StringOps::to_string(player->getPlayerIndex()) + ":" + player->getPlayerName();
	lua_pushstring(functionState, stringRepresentation.c_str());
	return 1;
}

int LuaGame::l_playerGetPosition(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	Mortal* player = PuppetMaster::getPlayer(playerIndex);
	Vec2d playerPosition = player->getRealTilePosition();
	
	pushVector(functionState, playerPosition);

	return 1;
}

int LuaGame::l_playerGetName(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	Mortal* player = PuppetMaster::getPlayer(index);
	lua_pushstring(functionState,player->getPlayerName().c_str());
	return 1;
}

int LuaGame::l_playerGetClass(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	Mortal* player = PuppetMaster::getPlayer(index);

	lua_pushstring(functionState, player->getClassName().c_str());
	return 1;
}

int LuaGame::l_playerGetSpeed(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	Mortal* player = PuppetMaster::getPlayer(index);

	double speed = player->getMovementSpeed();
	lua_pushnumber(functionState, speed);
	return 1;
}

int LuaGame::l_playerSetSpeed(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	double movementSpeed = lua_tonumber(functionState, 1);

	if (index == XClient::getInstance()->getPlayerIndex()) {
		Player::getInstance()->setMovementSpeed(movementSpeed);
	}

	return 0;
}

int LuaGame::l_playerGetHealth(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	if (index == XClient::getInstance()->getPlayerIndex()) {
		double health = Health::getInstance()->getHealth();
		lua_pushnumber(functionState, health);
		return 1;
	}
	else {
		// If the player you are refering to is not the player that is playing
		// We do not have a system for this, just return 0

		lua_pushnumber(functionState, 0);
		return 1;
	}
}

int LuaGame::l_playerSetHealth(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	if (index == XClient::getInstance()->getPlayerIndex()) {
		double health = lua_tointeger(functionState, 1);
		Health::getInstance()->setHealth(health);
	}

	return 0;
}

int LuaGame::l_playerHeal(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	double health = lua_tonumber(functionState, 1);

	// If the player is us
	if (index == XClient::getInstance()->getPlayerIndex()) {
		Health::getInstance()->heal(health);
	}
	// cant do anything yet if not us

	return 0;
}

int LuaGame::l_playerDamage(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	double health = lua_tonumber(functionState, 1);

	// if player is us
	if (index == XClient::getInstance()->getPlayerIndex()) {
		Health::getInstance()->damage(health);
	}

	return 0;
}

int LuaGame::l_playerTeleport(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(index, "index");

	Vec2d tilePos = toVector(functionState, 1);

	if (index == XClient::getInstance()->getPlayerIndex()) {
		Player::getInstance()->setTilePosition(tilePos);
	}
	return 0;
}

int LuaGame::l_playerGiveItem(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	if (lua_gettop(functionState) == 1) {
		string item = lua_tostring(functionState, 1);
		if (XClient::getPlayerIndex() == playerIndex)
			Inventory::giveItem(item, 1);
		else
			XClient::getInstance()->giveItem(playerIndex, item, 1);
	}
	else if (lua_gettop(functionState) == 2) {
		string item = lua_tostring(functionState, 1);
		int quantity = lua_tointeger(functionState, 2);
		
		if (XClient::getPlayerIndex() == playerIndex)
			Inventory::giveItem(item, quantity);
		else
			XClient::getInstance()->giveItem(playerIndex, item, quantity);
	}
	return 0;
}

int LuaGame::l_playerTakeItem(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	if (lua_gettop(functionState) == 2) {
		std::string itemName = lua_tostring(functionState, 1);
		int quantity = lua_tointeger(functionState, 2);
		if (XClient::getPlayerIndex() == playerIndex)
			Inventory::takeItem(itemName, quantity);
		else
			XClient::getInstance()->giveItem(playerIndex, itemName, quantity);
	}
	else if (lua_gettop(functionState) == 1) {
		std::string itemName = lua_tostring(functionState, 1);
		if (XClient::getPlayerIndex() == playerIndex)
			Inventory::takeItem(itemName, 1);
		else
			XClient::getInstance()->takeItem(playerIndex, itemName, 1);
	}
	return 0;
}

int LuaGame::l_playerGetItemQuantity(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	if (lua_gettop(functionState) == 1) {
		std::string itemName = lua_tostring(functionState,1);
		
		// Currently does not have implementation if the inventory is NOT that of the current player
		if (XClient::getPlayerIndex() == playerIndex) {
			int quantity = Inventory::getQuantityOf(itemName);
			lua_pushinteger(functionState, quantity);
			return 1;
		}
	}
	return 0;
}

int LuaGame::l_playerCanCraftItem(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	if (lua_gettop(functionState) == 1) {
		std::string itemName = lua_tostring(functionState, 1);
		if (XClient::getPlayerIndex() == playerIndex) {
			lua_pushboolean(functionState, Inventory::canCraftItem(itemName));
			return 1;
		}
	}
	return 0;
}

int LuaGame::l_playerHasItemWithTag(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(playerIndex, "index");

	if (lua_gettop(functionState) == 1) {

		if (playerIndex == XClient::getPlayerIndex()) {
			std::string targetTag = lua_tostring(functionState, 1);

			std::unordered_map<std::string, InventoryItem> items = Inventory::getItems();
			//Loop over each item, and check for tag
			for (auto &item : items) {
				list<std::string> tags = item.second.tags;
				//Loop over each tag of the item
				for (auto &tag : tags) {
					if (tag == targetTag) {
						lua_pushboolean(functionState, TRUE);
						return 1;
					}
				}
			}
			lua_pushboolean(functionState, FALSE);
			return 1;
		}
	}
	return 0;
}
void LuaGame::pushCreature(lua_State* state, int id) {
	pushObject(state, "creature", {
		{ "toString",
		"Returns a string representation of the creature",
		{ {} },
		LUA_TSTRING,
		l_creatureToString},

		{ "moveTo",
		"Moves the creature towards the tile given, returns the time it would take",
		{ { { LUA_TVECTOR, "tilePosition" } },
		{ { LUA_TVECTOR, "tilePosition" }, { LUA_TNUMBER, "distance" } },
		{ { LUA_TVECTOR, "tilePosition" }, { LUA_TNUMBER, "distance" }, { LUA_TNUMBER, "speed" }  }},
		LUA_TNUMBER,
		l_creatureMoveTo},

		{ "moveOn",
		"Moves the creature onto the tile given",
		{ { { LUA_TVECTOR, "tilePosition" } },
		{ { LUA_TVECTOR, "tilePosition" }, { LUA_TNUMBER, "speed" } }},
		0,
		l_creatureMoveOn},

		{ "getPosition",
		"Returns the tile coordinates of the creature",
		{ { } },
		LUA_TVECTOR,
		l_creatureGetPosition},

		{ "getSpeed",
		"Returns the movement speed of the creature in tiles per second",
		{ {} },
		LUA_TNUMBER,
		l_creatureGetSpeed},

		{ "setSpeed",
		"Sets the movement speed of the creature in tiles per second",
		{ { { LUA_TNUMBER, "speed" } } },
		0,
		l_creatureGetSpeed},

		{ "getName",
		"Returns the name of the creature",
		{ {} },
		LUA_TSTRING,
		l_creatureGetName},

		{ "getAI",
		"Returns the AI script of the creature",
		{ {} },
		LUA_TSTRING,
		l_creatureGetAI},

		{ "hasTag",
		"Returns if the creature has a certain tag",
		{ { { LUA_TSTRING, "tag" } } },
		0,
		l_creatureHasTag},

		{ "getProperties",
		"Returns the properties table of the creature",
		{ {} },
		LUA_TTABLE,
		l_creatureGetProperties},
		
		{ "setProperties",
		"Sets the properties table of the creature",
		{ { { LUA_TTABLE, "propertiesTable" } } },
		0,
		l_creatureSetProperties}

	});

	lua_getmetatable(state, -1);

	lua_pushstring(state, "id");
	lua_pushinteger(state, id);
	lua_settable(state, -3);

	lua_setmetatable(state, -2);
}

int LuaGame::l_creatureConstruct(lua_State* functionState) {
	CHECK_ARGS;

	std::string creatureName = lua_tostring(functionState, 1);
	Vec2d spawnPosition = toVector(functionState, 2);

	Creature* creature = Creature::spawnCreature(creatureName, spawnPosition);
	
	if (XClient::getInstance())
		XClient::getInstance()->spawnCreature(creatureName, spawnPosition);

	pushCreature(functionState, creature->getGID());
	return 1;
}

int LuaGame::l_creatureToString(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);

	std::string stringRepresentation = "Creature " + creature->getCreatureName() + " at " + vectorToString(creature->getTilePosition());

	return 1;
}

int LuaGame::l_creatureMoveTo(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		Creature* creature = Creature::getWithId(creatureId);
		double time = creature->moveTo(position);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 2);
		Creature* creature = Creature::getWithId(creatureId);
		double time = creature->moveTo(position, distance);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 3) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 2);
		double speed = lua_tonumber(functionState, 3);
		Creature* creature = Creature::getWithId(creatureId);
		double time = creature->moveTo(position, distance, speed);
		lua_pushnumber(functionState, time);
		return 1;
	}
	return 0;
}

int LuaGame::l_creatureMoveOn(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		Creature* creature = Creature::getWithId(creatureId);
		double time = creature->moveOn(position);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if(lua_gettop(functionState) == 2){
		Vec2d position = toVector(functionState, 1);
		double speed = lua_tonumber(functionState, 2);
		Creature* creature = Creature::getWithId(creatureId);
		double time = creature->moveOn(position, speed);
		lua_pushnumber(functionState, time);
		return 1;
	}
	return 0;
}

int LuaGame::l_creatureGetPosition(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);
	Vec2d position = creature->getRealTilePosition();
	pushVector(functionState, position);
	return 1;
}

int LuaGame::l_creatureGetSpeed(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);
	double speed = creature->getMovementSpeed();
	lua_pushnumber(functionState, speed);
	return 1;
}

int LuaGame::l_creatureSetSpeed(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	if (lua_gettop(functionState) == 1) {
		double speed = lua_tonumber(functionState, 1);
		Creature* creature = Creature::getWithId(creatureId);
		creature->setMovementSpeed(speed);
	}
	return 0;
}


int LuaGame::l_creatureGetName(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);
	std::string creatureName = creature->getName();
	lua_pushstring(functionState, creatureName.c_str());
	return 1;
}

int LuaGame::l_creatureGetAI(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);
	std::string creatureAI = creature->getCreatureAI();
	lua_pushstring(functionState, creatureAI.c_str());
	return 1;
}

int LuaGame::l_creatureHasTag(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	if (lua_gettop(functionState) == 1) {
		std::string tag = lua_tostring(functionState, 1);
		Creature* creature = Creature::getWithId(creatureId);
		lua_pushboolean(functionState, creature->hasTag(tag));
		return 1;
	}
}

int LuaGame::l_creatureGetProperties(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	Creature* creature = Creature::getWithId(creatureId);
	std::string properties = creature->getCreatureProperties();
	unPickleTable(functionState, properties);
	return 1;
}

int LuaGame::l_creatureSetProperties(lua_State* functionState) {
	CHECK_ARGS;
	GET_META_INT(creatureId, "id");

	if (lua_gettop(functionState) == 1) {
		std::string table = pickleTable(functionState, 1);
		Creature* creature = Creature::getWithId(creatureId);

		creature->setCreatureProperties(table);
	}
	return 0;
}

int LuaGame::l_playSong(lua_State* functionState) {
	CHECK_ARGS;

	if(lua_gettop(functionState) == 1){
		std::string songName = lua_tostring(functionState, 1);
		Music::play(songName);
	}
	return 0;
}

int LuaGame::l_spawnParticles(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 2) {
		std::string particleSystemName = lua_tostring(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		Particles::spawnParticles(particleSystemName, position);
	}
	return 0;
}

int LuaGame::l_print(lua_State* functionState) {

	int argCount = lua_gettop(functionState);
	
	for (int argIndex = 1; argIndex <= argCount; argIndex++) {

		std::string message;
		if (lua_isstring(functionState, argIndex))
			message = lua_tostring(functionState, argIndex);
		else if (lua_istable(functionState,argIndex))
			message = pickleTable(functionState, argIndex);
		else
			message = getType(functionState, argIndex);

		if (LuaTerminal::getInstance()) {
			LuaTerminal::getInstance()->print(message);
		}
	}
	return 0;
}

int LuaGame::l_clear(lua_State* functionState) {
	CHECK_ARGS;

	if (LuaTerminal::getInstance()) {
		LuaTerminal::getInstance()->clear();
	}
	return 0;
}

