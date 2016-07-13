#include "LuaCreature.h"
#include "CCPlatformMacros.h"
#include "MyMath.h"
#include "Creatures.h"
#include "Arena.h"
#include "Debug.h"

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

		Debugger::logError(lua_tostring(mainState, 1), DEBUG_LUA);
		lua_pop(mainState, 1);
	}

}

void LuaCreature::addFunctions(lua_State* mainState) {

	LuaGame::addFunctions(mainState);

	NEW_FUNCTION("getPosition", l_getCreaturePosition);
	NEW_FUNCTION("getRealPosition", l_getCreatureRealPosition);
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
	if(!assertArguments(functionState, "getPosition",
				{})) return 0;


	Creature* creatureReference = getCreatureReference(functionState);
	Vec2i tilePosition = creatureReference->getTilePosition();
	pushVector(functionState, tilePosition);

	return 1;
}

int LuaCreature::l_getCreatureRealPosition(lua_State* functionState) {
	if(!assertArguments(functionState, "getRealPosition", {})) return 0;

	Creature* creatureReference = getCreatureReference(functionState);
	Vec2 realTilePosition = creatureReference->getRealTilePosition();
	pushVector(functionState, realTilePosition);

	return 1;
}

int LuaCreature::l_creatureMoveOn(lua_State* functionState) {
	
	if(!assertArguments(functionState, "moveOn",{ 
				{ LUA_TTABLE },
				{ LUA_TTABLE, LUA_TNUMBER}
				})) return 0;

	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		creatureReference->moveOn(position);
	}
	else if(lua_gettop(functionState) == 2){
		Vec2d position = toVector(functionState, 1);
		double speed  = lua_tonumber(functionState, 2);
		creatureReference->moveOn(position, speed);
	}
	return 0;
}

int LuaCreature::l_newProximityState(lua_State* functionState) {
	
	if(!assertArguments(functionState, "States.newProximityState", {
				{LUA_TSTRING, LUA_TNUMBER}
				})) return 0;

	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double distance = lua_tonumber(functionState, 2);
		creatureReference->newProximityState(distance, functionName);
	}
	return 0;
}

int LuaCreature::l_newUpdateState(lua_State* functionState) {
	
	if(!assertArguments(functionState, "States.newUpdateState", {
				{LUA_TSTRING}
				})) return 0;

	if(lua_gettop(functionState) == 1){
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		creatureReference->newUpdateState(functionName);
	}
	return 0;
}

int LuaCreature::l_newClockState(lua_State* functionState) {

	if(!assertArguments(functionState, "States.newClockState", {
				{LUA_TSTRING, LUA_TNUMBER}
				})) return 0;

	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double clock = lua_tonumber(functionState, 2);
		creatureReference->newClockState(clock,functionName);
		
	}
	return 0;
}

int LuaCreature::l_deleteState(lua_State* functionState) {

	if(!assertArguments(functionState, "States.deleteState", {
				{LUA_TSTRING}
				})) return 0;

	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string stateName = lua_tostring(functionState, 1);
		creatureReference->deleteState(stateName);
	}
	return 0;
}

int LuaCreature::l_setCreatureMovementSpeed(lua_State* functionState) {
	if(!assertArguments(functionState, "setMovementSpeed", {
				{ LUA_TNUMBER }
				})) return 0;

	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = lua_tonumber(functionState, 1);
		creatureReference->setMovementSpeed(movementSpeed);
	}
	return 0;
}

int LuaCreature::l_getCreatureMovementSpeed(lua_State* functionState) {

	if(!assertArguments(functionState, "getMovementSpeed", {})) return 0;

	if (lua_gettop(functionState) == 0) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = creatureReference->getMovementSpeed();
		lua_pushnumber(functionState, movementSpeed);
		return 1;
	}
	return 0;
}

int LuaCreature::l_creatureMoveTo(lua_State* functionState) {

	if(!assertArguments(functionState, "moveTo",{
				{ LUA_TTABLE },
				{ LUA_TTABLE, LUA_TNUMBER },
				{ LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER}
				})) return 0;

	Creature* creatureReference = getCreatureReference(functionState);
	if(lua_gettop(functionState) == 1){
		Vec2d position = toVector(functionState, 1);
		creatureReference->moveTo(position);
	}
	if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 3);
		creatureReference->moveTo(position, distance);
	}
	else if (lua_gettop(functionState) == 3) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 2);
		double speed = lua_tonumber(functionState, 3);
		creatureReference->moveTo(position, distance, speed);
	}
	return 0;
}

int LuaCreature::l_despawn(lua_State* functionState) {

	if(!assertArguments(functionState, "despawn", {})) return 0;

	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 0) {
		creatureReference->despawn();
	}
	return 0;
}
