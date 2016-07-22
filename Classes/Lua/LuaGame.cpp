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

	// Terminal
	NEW_FUNCTION("print", l_print);
	NEW_FUNCTION("clear", l_clear);

	lua_pushinteger(mainState, UNTOUCHED);
	lua_setglobal(mainState, "UNTOUCHED");

	lua_pushinteger(mainState, DESTROYED);
	lua_setglobal(mainState, "DESTROYED");

	lua_pushinteger(mainState, DEPLETED);
	lua_setglobal(mainState, "DEPLETED");
	
	luaL_Reg creature_static[] = {
		{ "spawnCreature", l_spawnCreature },
		{ "getType", l_getCreatureType },
		{ NULL, NULL }
	};

	luaL_Reg creature_member[] = {
		{ "toString", l_creatureToString },
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
		{ "getType", l_getInventoryType},
		{ NULL, NULL }
	};

	luaL_Reg inventory_member[] = {
		{ "toString", l_inventoryToString },
		{ "giveItem",l_givePlayerItem },
		{ "getQuantityOf",l_getQuantityOf },
		{ "takeItem",l_takeItem },
		{ "canCraftItem", l_canCraftItem },
		{ "hasItemWithTag", l_hasItemWithTag },
		{ NULL, NULL }
	};

	luaL_Reg player_static[] = {
		{ "getByIndex", l_getPlayerByIndex },
		{ "getType", l_getPlayerType },
		{ NULL,NULL }
	};

	luaL_Reg player_member[] = {
		{ "toString", l_playerToString },
		{ "getPosition", l_getPlayerPosition },
		{ "getRealPosition", l_getPlayerRealPosition },
		{ "getPlayerName", l_getPlayerName },
		{ "getClassName", l_getClassName },
		{ "getMovementSpeed", l_getPlayerMovementSpeed },
		{ "setMovementSpeed", l_setPlayerMovementSpeed },
		{ "getInventory",l_getPlayerInventory },
		{ "getHealth", l_getPlayerHealth},
		{ "heal", l_healPlayer },
		{ "damage", l_damagePlayer },
		{ "teleport", l_teleportPlayer },
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
		{ LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 1) { // If there is one arguments
		Vec2d position = toVector(functionState, 1);
		string name = Arena::getMapInstance()->getTileNameAt(position);
		lua_pushstring(functionState, name.c_str());
		return 1;
	}
	return 0;
}

int LuaGame::l_findRegionAt(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.getTileRegionAt", {
		{ LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 1) { 
		Vec2d position = toVector(functionState, 1);
		string name = Arena::getMapInstance()->getTileRegionAt(position);
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
	
	pushVector(functionState, playerPosition);

	return 1;
}

int LuaGame::l_getPlayerRealPosition(lua_State* functionState) {

	if (!assertArguments(functionState, "Player:getRealPosition", {
		{}
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);
	Vec2 playerLocation = player->getRealTilePosition();

	pushVector(functionState, playerLocation);

	return 1;
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
		{ LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		Arena::getMapInstance()->destroyTile(position);
	}
	return 0;
}

int LuaGame::l_setTileState(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.setTileState", {
		{ LUA_TVECTOR , LUA_TNUMBER}
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);
		int state = lua_tointeger(functionState, 2);
		Arena::getMapInstance()->setTileState(position, state);
	}

	return 0;
}

int LuaGame::l_getTileState(lua_State* functionState) {

	if (!assertArguments(functionState, "Arena.getTileState", {
		{ LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		int state = Arena::getMapInstance()->getTileState(position);
		
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
		{ LUA_TVECTOR, LUA_TTABLE }
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
	
		std::string pickledTable = Arena::getMapInstance()->getTileProperties(position);
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
		{ LUA_TVECTOR , LUA_TTABLE }
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);

		std::string pickledTable = pickleTable(functionState, 2);
						
		// send everyone the changes and also change our own map
		XClient::getInstance()->setTileProperties(position, pickledTable);
		Arena::getMapInstance()->setTileProperies(position, pickledTable);
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

int LuaGame::l_setPlayerMovementSpeed(lua_State* functionState) {
	if (!assertArguments(functionState, "Player:setMovementSpeed", {
		{ LUA_TNUMBER }
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	double movementSpeed = lua_tonumber(functionState, 2);

	if (*luaPlayer == XClient::getInstance()->getPlayerIndex()) {
		Player::getInstance()->setMovementSpeed(movementSpeed);
	}

	return 0;
}

int LuaGame::l_spawnCreature(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature.spawnCreature", {
		{ LUA_TSTRING, LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 2) {
		std::string name = lua_tostring(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		Creature* creature = Creature::spawnCreature(name, position);
		XClient::getInstance()->spawnCreature(name, position);
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
		{ LUA_TVECTOR },
		{ LUA_TVECTOR , LUA_TNUMBER},
		{ LUA_TVECTOR, LUA_TNUMBER, LUA_TNUMBER}
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(position);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 3) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(position, distance);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if (lua_gettop(functionState) == 4) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		double speed = lua_tonumber(functionState, 4);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveTo(position, distance, speed);
		lua_pushnumber(functionState, time);
		return 1;
	}
	return 0;
}

int LuaGame::l_creatureMoveOn(lua_State* functionState) {

	if (!assertArguments(functionState, "Creature:moveOn", {
		{ LUA_TVECTOR },
		{ LUA_TVECTOR, LUA_TNUMBER }
	}, true)) return 0;

	if (lua_gettop(functionState) == 2) {
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveOn(position);
		lua_pushnumber(functionState, time);
		return 1;
	}
	else if(lua_gettop(functionState) == 3){
		LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
		Vec2d position = toVector(functionState, 2);
		double speed = lua_tonumber(functionState, 3);
		Creature* creature = Creature::getWithId(*luaCreature);
		double time = creature->moveOn(position, speed);
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
		pushVector(functionState, position);
		return 1;
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
		pushVector(functionState, position);
		return 1;
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

int LuaGame::l_getPlayerHealth(lua_State* functionState) {
	if (!assertArguments(functionState, "Player:getHealth", { {} }, true)) return 0;

	LuaPlayerObject* player = luaW_check<LuaPlayerObject>(functionState, 1);

	if (*player == XClient::getInstance()->getPlayerIndex()) {
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

int LuaGame::l_healPlayer(lua_State* functionState) {
	if (!assertArguments(functionState, "Player:heal", {
		{ LUA_TNUMBER }
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	double health = lua_tonumber(functionState, 2);

	// If the player is us
	if (*luaPlayer == XClient::getInstance()->getPlayerIndex()) {
		Health::getInstance()->heal(health);
	}
	// cant do anything yet if not us

	return 0;
}

int LuaGame::l_damagePlayer(lua_State* functionState) {
	if (!assertArguments(functionState, "Player:damage", {
		{ LUA_TNUMBER }
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	double damageHealth = lua_tonumber(functionState, 2);

	// if player is us
	if (*luaPlayer == XClient::getInstance()->getPlayerIndex()) {
		Health::getInstance()->damage(damageHealth);
	}

	return 0;
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
		{ LUA_TVECTOR }
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
		{ LUA_TSTRING, LUA_TVECTOR }
	})) return 0;

	if (lua_gettop(functionState) == 3) {
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
	if (!assertArguments(functionState, "clear", {
		{}
	})) return 0;

	if (LuaTerminal::getInstance()) {
		LuaTerminal::getInstance()->clear();
	}
	return 0;
}
int LuaGame::l_teleportPlayer(lua_State* functionState) {
	if (!assertArguments(functionState, "player:teleport", {
		{ LUA_TVECTOR }
	}, true)) return 0;

	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Vec2d tilePos = toVector(functionState, 2);

	if (*luaPlayer == XClient::getInstance()->getPlayerIndex()) {
		Player::getInstance()->setTilePosition(tilePos);
	}
	return 0;
}

int LuaGame::l_getCreatureType(lua_State* functionState) {
	lua_pushstring(functionState, "Creature");
	return 1;
}

int LuaGame::l_getInventoryType(lua_State* functionState) {
	lua_pushstring(functionState, "Inventory");
	return 1;
}

int LuaGame::l_getPlayerType(lua_State* functionState){
	lua_pushstring(functionState, "Player");
	return 1;
}

int LuaGame::l_inventoryToString(lua_State* functionState) {
	LuaInventoryObject* inventory = luaW_check<LuaInventoryObject>(functionState, 1);
	LuaPlayerObject luaPlayer = (int)*inventory;
	Mortal* player = PuppetMaster::getPlayer(luaPlayer);
	
	std::string stringRepresentation = "Inventory of Player " +StringOps::to_string(player->getPlayerIndex()) + ":" + player->getPlayerName();
	lua_pushstring(functionState, stringRepresentation.c_str());
	return 1;
}

int LuaGame::l_creatureToString(lua_State* functionState) {
	LuaCreatureObject* luaCreature = luaW_check<LuaCreatureObject>(functionState, 1);
	Creature* creature = Creature::getWithId(*luaCreature);

	std::string stringRepresentation = "Creature " + creature->getCreatureName() + " at " + vectorToString(creature->getTilePosition());

	return 1;
}

int LuaGame::l_playerToString(lua_State* functionState) {
	LuaPlayerObject* luaPlayer = luaW_check<LuaPlayerObject>(functionState, 1);
	Mortal* player = PuppetMaster::getPlayer(*luaPlayer);
	
	std::string stringRepresentation = "Player " +StringOps::to_string(player->getPlayerIndex()) + ":" + player->getPlayerName();
	lua_pushstring(functionState, stringRepresentation.c_str());
	return 1;
}