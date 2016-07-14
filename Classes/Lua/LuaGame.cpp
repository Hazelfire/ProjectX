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



void LuaGame::addFunctions(lua_State* mainState) {
	LuaInterpreter::addFunctions(mainState);
	//Arena
	NEW_TABLE_SIZE(7);

	NEW_ROW("getTileNameAt", l_findNameAt);
	NEW_ROW("getTileRegionAt", l_findRegionAt);
	NEW_ROW("destroyTile", l_destroyTile);
	NEW_ROW("setTileState", l_setTileState);
	NEW_ROW("getTileState", l_getTileState);
	NEW_ROW("getTileProperties", l_getTileProperties);
	NEW_ROW("setTileProperties", l_setTileProperties);

	NAME_TABLE("Arena");

	// Music
	NEW_TABLE_SIZE(2);
	
	NEW_ROW("playSong", l_playSong);
	NEW_ROW("playRegionSong", l_playSongRegion);

	NAME_TABLE("Music");

	// Particles
	NEW_TABLE_SIZE(1);

	NEW_ROW("spawnParticles", l_spawnParticles);
	
	NAME_TABLE("Particles");

	lua_pushinteger(mainState, UNTOUCHED);
	lua_setglobal(mainState, "UNTOUCHED");

	lua_pushinteger(mainState, DESTROYED);
	lua_setglobal(mainState, "DESTROYED");

	lua_pushinteger(mainState, DEPLETED);
	lua_setglobal(mainState, "DEPLETED");
	
	luaL_Reg creature_static[] = {
		{ "spawnCreature", l_spawnCreature },
		{ NULL, NULL }
	};

	luaL_Reg creature_member[] = {
		{ "moveTo", l_creatureMoveTo },
		{ "moveOn",l_creatureMoveOn },
		{ "getPosition", l_getCreaturePosition },
		{ "getRealPosition", l_getCreatureRealPosition },
		{ "setMovementSpeed", l_setCreatureMovementSpeed },
		{ "getMovementSpeed", l_getCreatureMovementSpeed },
		{ "getName", l_getCreatureName },
		{ "getAI", l_getCreatureAI },
		{ "hasTag", l_creatureHasTag },
		{ "getProperties", l_getCreatureProperties },
		{ "setPropeties", l_setCreatureProperties },
		{ NULL, NULL }
	};

	luaL_Reg inventory_static[] = {
		{ NULL, NULL }
	};

	luaL_Reg inventory_member[] = {
		{ "giveItem",l_givePlayerItem },
		{ "getQuantityOf",l_getQuantityOf },
		{ "takeItem",l_takeItem },
		{ "canCraftItem", l_canCraftItem },
		{ "hasItemWithTag", l_hasItemWithTag },
		{ NULL, NULL }
	};

	luaL_Reg player_static[] = {
		{ "getByIndex", l_getPlayerByIndex },
		{ NULL,NULL }
	};

	luaL_Reg player_member[] = {
		{ "getPosition", l_getPlayerPosition },
		{ "getRealPosition", l_getPlayerRealPosition },
		{ "getPlayerName", l_getPlayerName },
		{ "getClassName", l_getClassName },
		{ "getMovementSpeed", l_getPlayerMovementSpeed },
		{ "getInventory",l_getPlayerInventory },
		{ NULL, NULL }
	};

	luaW_setfuncs<LuaCreatureObject>(mainState, "Creature", creature_static, creature_member);
	lua_setglobal(mainState, "Creature");

	luaW_setfuncs<LuaPlayerObject>(mainState, "Player", player_static, player_member);
	lua_setglobal(mainState, "Player");

	luaW_setfuncs<LuaInventoryObject>(mainState, "Inventory", inventory_static, inventory_member);
	lua_setglobal(mainState, "Inventory");
}

void LuaGame::callWithPlayer(std::string function, int player) {
	lua_getglobal(m_mainState, function.c_str());

	luaW_push<LuaPlayerObject>(m_mainState, new LuaPlayerObject(player));

	int error = lua_pcall(m_mainState,1, 0, 0);
	if (error) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		CCLOG(lua_tostring(m_mainState, -1));
#endif
		lua_pop(m_mainState, 1);
	}
}

int LuaGame::l_findNameAt(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.getTileNameAt", {
		{LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 2) { // If there is 2 arguments
		int x = lua_tonumber(functionState, 1);
		int y = lua_tonumber(functionState, 2);
		string name = Arena::getMapInstance()->getTileNameAt(Vec2i(x, y));
		lua_pushstring(functionState, name.c_str());
		return 1;
	}
	return 0;
}

int LuaGame::l_findRegionAt(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.getTileRegionAt", {
		{ LUA_TNUMBER, LUA_TNUMBER }
	})) return 0;

	if (lua_gettop(functionState) == 2) { // If there is 2 arguments
		int x = lua_tonumber(functionState, 1);
		int y = lua_tonumber(functionState, 2);
		string name = Arena::getMapInstance()->getTileRegionAt(Vec2i(x, y));
		lua_pushstring(functionState, name.c_str());
		return 1;
	}
	return 0;
}

int LuaGame::l_getPlayerPosition(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getPosition", {
		{}
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);
	Vec2i playerPosition = player->getTilePosition();
	
	lua_pushinteger(functionState, playerPosition.x);
	lua_pushinteger(functionState, playerPosition.y);

	return 2;
}

int LuaGame::l_getPlayerRealPosition(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getRealPosition", {
		{}
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);
	Vec2 playerLocation = player->getRealTilePosition();

	lua_pushinteger(functionState, playerLocation.x);
	lua_pushinteger(functionState, playerLocation.y);

	return 2;
}

int LuaGame::l_givePlayerItem(lua_State* functionState) {

	if (!assertArguments(functionState, "Inventory:giveItem", {
		{LUA_TSTRING},
		{LUA_TSTRING, LUA_TNUMBER}
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		string item = lua_tostring(functionState, 2);
		if (XClient::getPlayerIndex() == *luaInventory)
			Inventory::giveItem(item, 1);
		else
			XClient::getInstance()->giveItem(*luaInventory,item, 1);
	}
	else if (lua_gettop(functionState) == 3) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		string item = lua_tostring(functionState, 2);
		int quantity = lua_tointeger(functionState, 3);
		
		if (XClient::getPlayerIndex() == *luaInventory)
			Inventory::giveItem(item, quantity);
		else
			XClient::getInstance()->giveItem(*luaInventory, item, quantity);
	}
	return 0;
}

int LuaGame::l_getPlayerName(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getPlayerName", {
		{}
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);
	lua_pushstring(functionState,player->getPlayerName().c_str());
	return 1;
}

int LuaGame::l_getClassName(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getClassName", {
		{}
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);

	lua_pushstring(functionState, player->getClassName().c_str());
	return 1;
}

int LuaGame::l_destroyTile(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.destroyTile", {
		{LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		Arena::getMapInstance()->destroyTile(Vec2i(x,y));
	}
	return 0;
}

int LuaGame::l_setTileState(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.setTileState", {
		{LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 3) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int state = lua_tointeger(functionState, 3);
		Arena::getMapInstance()->setTileState(Vec2i(x,y), state);
	}

	return 0;
}

int LuaGame::l_getTileState(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.getTileState", {
		{ LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int state = Arena::getMapInstance()->getTileState(Vec2i(x, y));
		
		lua_pushinteger(functionState, state);
		return 1;
	}
	return 0;
}

int LuaGame::l_getQuantityOf(lua_State* functionState) {

	if (!assertArguments(functionState, "Inventory:getQuantityOf", {
		{ LUA_TSTRING }
	},true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		std::string itemName = lua_tostring(functionState,2);
		
		// Currently does not have implementation if the inventory is NOT that of the current player
		if (XClient::getPlayerIndex() == *luaInventory) {
			int quantity = Inventory::getQuantityOf(itemName);
			lua_pushinteger(functionState, quantity);
			return 1;
		}
	}
	return 0;
}

int LuaGame::l_takeItem(lua_State* functionState) {

	if (!assertArguments(functionState, "Inventory:takeItem", {
		{ LUA_TSTRING, LUA_TNUMBER },
		{LUA_TSTRING}
	},true)) return 0;

	if (lua_gettop(functionState) == 3) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		std::string itemName = lua_tostring(functionState, 2);
		int quantity = lua_tointeger(functionState, 3);
		if (XClient::getPlayerIndex() == *luaInventory)
			Inventory::takeItem(itemName, quantity);
		else
			XClient::getInstance()->giveItem(*luaInventory, itemName, quantity);
	}
	else if (lua_gettop(functionState) == 2) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		std::string itemName = lua_tostring(functionState, 2);
		if (XClient::getPlayerIndex() == *luaInventory)
			Inventory::takeItem(itemName, 1);
		else
			XClient::getInstance()->takeItem(*luaInventory, itemName, 1);
	}
	return 0;
}

int LuaGame::l_canCraftItem(lua_State* functionState) {

	if (!assertArguments(functionState, "Inventory:canCraftItem", {
		{ LUA_TSTRING}
	},true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		std::string itemName = lua_tostring(functionState, 2);
		if (XClient::getPlayerIndex() == *luaInventory) {
			lua_pushboolean(functionState, Inventory::canCraftItem(itemName));
			return 1;
		}
	}
	return 0;
}

int LuaGame::l_getTileProperties(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena:getTileProperties", {
		{ LUA_TNUMBER, LUA_TNUMBER }
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
	
		std::string pickledTable = Arena::getMapInstance()->getTileProperties(Vec2i(x, y));
		if(pickledTable.empty())
			lua_newtable(functionState);
		else
			unPickleTable(functionState, pickledTable);
		return 1;
	}				
	return 0;
}

int LuaGame::l_setTileProperties(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena:setTileProperties", {
		{ LUA_TNUMBER, LUA_TNUMBER, LUA_TTABLE }
	})) return 0;

	if (lua_gettop(functionState) == 3) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);

		std::string pickledTable = pickleTable(functionState, 3);
						
		// send everyone the changes and also change our own map
		XClient::getInstance()->setTileProperties(Vec2i(x, y), pickledTable);
		Arena::getMapInstance()->setTileProperies(Vec2i(x, y), pickledTable);
	}
	return 0;
}

int LuaGame::l_getPlayerMovementSpeed(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getMovementSpeed", {
		{ }
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);

	double speed = player->getMovementSpeed();
	lua_pushnumber(functionState, speed);
	return 1;
}

int LuaGame::l_spawnCreature(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature.spawnCreature", {
		{ LUA_TSTRING,LUA_TNUMBER,LUA_TNUMBER }
	})) return 0;

	if (lua_gettop(functionState) == 3) {
		std::string name = lua_tostring(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		Creature* creature = Creature::spawnCreature(name, Vec2i(x, y));
		XClient::getInstance()->spawnCreature(name, Vec2i(x,y));
		LuaCreatureObject* luaCreature = new LuaCreatureObject(creature->getGID());
		luaW_push<LuaCreatureObject>(functionState,luaCreature);
		return 1;
	}
	return 0;
}

int LuaGame::l_playSong(lua_State* functionState) {

	if (!assertArguments(functionState, "Music.playSong", {
		{ LUA_TSTRING }
	})) return 0;

	if(lua_gettop(functionState) == 1){
		std::string songName = lua_tostring(functionState, 1);
		Music::play(songName);
	}
	return 0;
}

int LuaGame::l_playSongRegion(lua_State* functionState) {

	if (!assertArguments(functionState, "Music.playRegionSong", {
		{ LUA_TSTRING }
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		std::string regionName = lua_tostring(functionState, 1);
		Music::playRegion(regionName);
	}
	return 0;
}


int LuaGame::l_hasItemWithTag(lua_State* functionState) {

	if (!assertArguments(functionState, "Inventory:hasItemWithTag", {
		{ LUA_TSTRING }
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {

		LuaInventoryObject* luaInventory = luaW_check<LuaInventoryObject>(functionState, 1);
		if (*luaInventory == XClient::getPlayerIndex()) {
			std::string targetTag = lua_tostring(functionState, 1);

			std::unordered_map<std::string, InventoryItem> items = Inventory::getItems();
			//Loop over each item, and check for tag
			for (auto &item : items) {
				list<std::string> tags = item.second.tags;
				//Loop over each tag of the item
				for (auto &tag : tags) {
					if (tag == targetTag) {
						lua_pushboolean(functionState, 1);
						return 1;
					}
				}
			}
			lua_pushboolean(functionState, 0);
			return 1;
		}
	}
	return 0;
}

int LuaGame::l_creatureMoveTo(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:moveTo", {
		{ LUA_TNUMBER, LUA_TNUMBER },
		{LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER},
		{LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER}
	}, true)) return 0;

	if (lua_gettop(functionState) == 3) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(Vec2i(x, y));
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 4) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		int distance = lua_tointeger(functionState, 4);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(Vec2i(x, y), distance);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 5) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		int distance = lua_tointeger(functionState, 4);
		double speed = lua_tonumber(functionState, 5);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(Vec2i(x, y), distance, speed);
		lua_pushnumber(functionState, time);
		return 1;
	}
	return 0;
}

int LuaGame::l_creatureMoveOn(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:moveOn", {
		{ LUA_TNUMBER, LUA_TNUMBER },
		{ LUA_TNUMBER, LUA_TNUMBER, LUA_TNUMBER }
	}, true)) return 0;

	if (lua_gettop(functionState) == 3) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveOn(Vec2i(x, y));
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if(lua_gettop(functionState) == 4){
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		int x = lua_tointeger(functionState, 2);
		int y = lua_tointeger(functionState, 3);
		double speed = lua_tonumber(functionState, 4);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveOn(Vec2i(x, y), speed);
		lua_pushnumber(functionState, time);
		return 1;
	}
	return 0;
}

int LuaGame::l_getCreaturePosition(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getPosition", {
		{}
	}, true)) return 0;

	if (lua_gettop(functionState) == 1) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Creature* creature = Creature::getWithId(*luaCreature);
		Vec2i position = creature->getTilePosition();
		lua_pushinteger(functionState, position.x);
		lua_pushinteger(functionState, position.y);
		return 2;
	}
	return 0;
}

int LuaGame::l_getCreatureRealPosition(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getRealPosition", {
		{}
	}, true)) return 0;

	if (lua_gettop(functionState) == 1) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Creature* creature = Creature::getWithId(*luaCreature);
		Vec2 position = creature->getRealTilePosition();
		lua_pushnumber(functionState, position.x);
		lua_pushnumber(functionState, position.y);
		return 2;
	}
	return 0;
}

int LuaGame::l_setCreatureMovementSpeed(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:setMovementSpeed", {
		{LUA_TNUMBER}
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		double speed = lua_tonumber(functionState, 2);
		Creature* creature = Creature::getWithId(*luaCreature);
		creature->setMovementSpeed(speed);
	}
	return 0;
}

int LuaGame::l_getCreatureMovementSpeed(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getMovementSpeed", {
		{ }
	}, true)) return 0;

	if (lua_gettop(functionState) == 1) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Creature* creature = Creature::getWithId(*luaCreature);
		double speed = creature->getMovementSpeed();
		lua_pushnumber(functionState, speed);
		return 1;
	}
	return 0;
}

int LuaGame::l_getPlayerByIndex(lua_State* functionState) {

	if (!assertArguments(functionState, "Player.getByIndex", {
		{ LUA_TNUMBER }
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		int index = lua_tointeger(functionState, 1);
		luaW_push<LuaPlayerObject>(functionState, new LuaPlayerObject(index));
		return 1;
	}
	return 0;
}

int LuaGame::l_getPlayerInventory(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getInventory", {
		{ }
	}, true)) return 0;

	LuaInventoryObject* inventory = luaW_check<LuaInventoryObject>(functionState, 1);
	luaW_push<LuaInventoryObject>(functionState, inventory);
	return 1;
}

int LuaGame::l_getCreatureName(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getCreatureName", {
		{}
	}, true)) return 0;

	LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
	Creature* creature = Creature::getWithId(*luaCreature);
	std::string creatureName = creature->getName();
	lua_pushstring(functionState, creatureName.c_str());
	return 1;
}

int LuaGame::l_getCreatureAI(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getCreatureAI", {
		{}
	}, true)) return 0;

	LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
	Creature* creature = Creature::getWithId(*luaCreature);
	std::string creatureAI = creature->getCreatureAI();
	lua_pushstring(functionState, creatureAI.c_str());
	return 1;
}

int LuaGame::l_creatureHasTag(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:hasTag", {
		{ LUA_TSTRING }
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		std::string tag = lua_tostring(functionState, 2);
		Creature* creature = Creature::getWithId(*luaCreature);
		lua_pushboolean(functionState, creature->hasTag(tag));
		return 1;
	}
	return 0;
}

int LuaGame::l_getCreatureProperties(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:getProperties", {
		{}
	}, true)) return 0;

	LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
	Creature* creature = Creature::getWithId(*luaCreature);
	std::string properties = creature->getCreatureProperties();
	unPickleTable(functionState, properties);
	return 1;
}

int LuaGame::l_setCreatureProperties(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:setProperties", {
		{LUA_TTABLE}
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		std::string table = pickleTable(functionState, 2);
		Creature* creature = Creature::getWithId(*luaCreature);

		creature->setCreatureProperties(table);
	}
	return 0;
}

int LuaGame::l_spawnParticles(lua_State* functionState) {
	if (!assertArguments(functionState, "Particles.spawnParticles", {
		{LUA_TSTRING, LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 3) {
		std::string particleSystemName = lua_tostring(functionState, 1);
		float x = lua_tonumber(functionState, 2);
		float y = lua_tonumber(functionState, 3);
		Particles::spawnParticles(particleSystemName, Vec2f(x, y));
	}
	return 0;
}
