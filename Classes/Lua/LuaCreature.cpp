#include "LuaCreature.h"
#include "CCPlatformMacros.h"
#include "MyMath.h"
#include "Creatures.h"
#include "Arena.h"

#define CREATURE_GID "CREATURE_GID"


static Creature* getCreatureReference(lua_State* functionState) {
	// Request the GID
	lua_getglobal(functionState, CREATURE_GID);
	
	// Get the GID
	int gid = lua_tointeger(functionState, -1);

	// Remove the GID
	lua_pop(functionState, 1);

	// Return the associated Creature - OOP in a nutshell!
	return Creature::getWithId(gid);
}

void LuaCreature::init(short int creatureGid) {
	m_creatureGID = creatureGid;
}

void LuaCreature::start(lua_State* mainState) {
	lua_pushstring(mainState, "start");
	int error =lua_pcall(mainState, 1, 0, 0);

	if (error!=0) {
#if ((CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID))
		CCLOG(lua_tostring(mainState, -1));
#endif
		lua_pop(mainState, 1);
	}

}

void LuaCreature::addFunctions(lua_State* mainState) {

	LuaGame::addFunctions(mainState);

	NEW_FUNCTION("getPosition", l_getCreaturePosition);
	NEW_FUNCTION("moveOn", l_creatureMoveOn);
	NEW_FUNCTION("setMovementSpeed", l_setCreatureMovementSpeed);
	NEW_FUNCTION("getMovementSpeed", l_getCreatureMovementSpeed);
	NEW_FUNCTION("moveTo", l_creatureMoveTo);
	NEW_FUNCTION("despawn", l_despawn);
	
	NEW_TABLE_SIZE(4);
	
	NEW_ROW("newProximityState", l_newProximityState);
	NEW_ROW("newUpdateState", l_newUpdateState);
	NEW_ROW("newClockState", l_newClockState);
	NEW_ROW("deleteState", l_deleteState);

	NAME_TABLE("States");
	lua_pushinteger(mainState, (short int)m_creatureGID);
	lua_setglobal(mainState, CREATURE_GID);
}

int LuaCreature::l_getCreaturePosition(lua_State* functionState) {
	Creature* creatureReference = getCreatureReference(functionState);
	Vec2i tilePosition = creatureReference->getTilePosition();
	lua_pushinteger(functionState, tilePosition.x);
	lua_pushinteger(functionState, tilePosition.y);
	
	return 2;
}

int LuaCreature::l_getCreatureRealPosition(lua_State* functionState) {
	Creature* creatureReference = getCreatureReference(functionState);
	Vec2 realTilePosition = creatureReference->getRealTilePosition();
	lua_pushnumber(functionState, realTilePosition.x);
	lua_pushnumber(functionState, realTilePosition.y);
	return 2;
}

int LuaCreature::l_creatureMoveOn(lua_State* functionState) {
	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		creatureReference->moveOn(Vec2i(x, y));
	}
	return 0;
}

int LuaCreature::l_newProximityState(lua_State* functionState) {
	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double distance = lua_tonumber(functionState, 2);
		creatureReference->newProximityState(distance, functionName);
	}
	return 0;
}

int LuaCreature::l_newUpdateState(lua_State* functionState) {
	if(lua_gettop(functionState) == 1){
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		creatureReference->newUpdateState(functionName);
	}
	return 0;
}

int LuaCreature::l_newClockState(lua_State* functionState) {
	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double clock = lua_tonumber(functionState, 2);
		creatureReference->newClockState(clock,functionName);
		
	}
	return 0;
}

int LuaCreature::l_deleteState(lua_State* functionState) {
	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string stateName = lua_tostring(functionState, 1);
		creatureReference->deleteState(stateName);
	}
	return 0;
}

int LuaCreature::l_setCreatureMovementSpeed(lua_State* functionState) {
	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = lua_tonumber(functionState, 1);
		creatureReference->setMovementSpeed(movementSpeed);
	}
	return 0;
}

int LuaCreature::l_getCreatureMovementSpeed(lua_State* functionState) {
	if (lua_gettop(functionState) == 0) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = creatureReference->getMovementSpeed();
		lua_pushnumber(functionState, movementSpeed);
		return 1;
	}
	return 0;
}

int LuaCreature::l_creatureMoveTo(lua_State* functionState) {
	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 3) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		creatureReference->moveTo(Vec2i(x, y), distance);
	}
	else if (lua_gettop(functionState) == 4) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		double speed = lua_tonumber(functionState, 4);
		creatureReference->moveTo(Vec2i(x, y), distance, speed);
	}
	return 0;
}

int LuaCreature::l_despawn(lua_State* functionState) {
	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 0) {
		creatureReference->despawn();
	}
	return 0;
}