#include "LuaCreature.h"
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

	pushCFunction(mainState,
		0,
		{ "getPosition",
		"Returns the position of the creature",
		{ { } },
		LUA_TVECTOR,
		l_getCreaturePosition });
	lua_setglobal(mainState, "getPosition");

	pushCFunction(mainState,
		0,
		{ "moveOn",
		"Moves the creature onto the tile",
		{ { { LUA_TVECTOR, "destination" } },
		{ { LUA_TVECTOR, "destination" }, { LUA_TNUMBER, "distance" } },
		{ { LUA_TVECTOR, "destination" }, { LUA_TNUMBER, "distance" }, { LUA_TNUMBER, "speed"} } },
		LUA_TNUMBER,
		l_creatureMoveOn});
	lua_setglobal(mainState, "moveOn");

	pushCFunction(mainState,
		0,
		{ "setSpeed",
		"Sets the movement speed of the creature in tiles per second",
		{ { { LUA_TNUMBER, "speed" } } },
		0,
		l_setCreatureMovementSpeed });
	lua_setglobal(mainState, "setSpeed");
	
	pushCFunction(mainState,
		0,
		{ "getSpeed",
		"Returns the movement speed of the creature in tiles per second",
		{ { } },
		LUA_TNUMBER,
		l_getCreatureMovementSpeed });
	lua_setglobal(mainState, "getSpeed");

	pushCFunction(mainState,
		0,
		{ "moveTo",
		"Moves the creature towards the tile given",
		{ { { LUA_TVECTOR, "destination" } },
		{ { LUA_TVECTOR, "destination" }, { LUA_TNUMBER, "distance" } },
		{ { LUA_TVECTOR, "destination" }, { LUA_TNUMBER, "distance" }, { LUA_TNUMBER, "speed" } } },
		LUA_TNUMBER,
		l_creatureMoveTo });
	lua_setglobal(mainState, "moveTo");

	pushCFunction(mainState,
		0,
		{ "despawn",
		"Despawns the creature",
		{ { } },
		0,
		l_despawn });
	lua_setglobal(mainState, "despawn");
	
	pushCFunction(mainState,
		0,
		{ "newProximityState",
		"Creates a new proximity state",
		{ { { LUA_TSTRING, "stateName" }, { LUA_TNUMBER, "distance" } } },
		0,
		l_newProximityState });
	lua_setglobal(mainState, "newProximityState");

	pushCFunction(mainState,
		0,
		{ "newUpdateState",
		"Creates a new update state",
		{ { { LUA_TSTRING, "stateName" } } },
		0,
		l_newUpdateState });
	lua_setglobal(mainState, "newUpdateState");
	
	pushCFunction(mainState,
		0,
		{ "newClockState",
		"Creates a new clock state",
		{ { { LUA_TSTRING, "stateName" }, { LUA_TNUMBER, "interval" } } },
		0,
		l_newClockState });
	lua_getglobal(mainState, "newClockState");

	pushCFunction(mainState,
		0,
		{ "deleteState",
		"Deletes the given state",
		{ { { LUA_TSTRING, "stateName" } } },
		0,
		l_deleteState });
	lua_setglobal(mainState, "deleteState");

	lua_pushinteger(mainState, (short int)m_creatureGID);
	lua_setglobal(mainState, CREATURE_GID);
}

int LuaCreature::l_getCreaturePosition(lua_State* functionState) {
	CHECK_ARGS;

	Creature* creatureReference = getCreatureReference(functionState);
	Vec2i tilePosition = creatureReference->getTilePosition();
	pushVector(functionState, tilePosition);

	return 1;
}

int LuaCreature::l_getCreatureRealPosition(lua_State* functionState) {
	CHECK_ARGS;

	Creature* creatureReference = getCreatureReference(functionState);
	Vec2 realTilePosition = creatureReference->getRealTilePosition();
	pushVector(functionState, realTilePosition);

	return 1;
}

int LuaCreature::l_creatureMoveOn(lua_State* functionState) {
	CHECK_ARGS;

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
	CHECK_ARGS;

	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double distance = lua_tonumber(functionState, 2);
		creatureReference->newProximityState(distance, functionName);
	}
	return 0;
}

int LuaCreature::l_newUpdateState(lua_State* functionState) {
	CHECK_ARGS;

	if(lua_gettop(functionState) == 1){
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		creatureReference->newUpdateState(functionName);
	}
	return 0;
}

int LuaCreature::l_newClockState(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 2) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string functionName = lua_tostring(functionState, 1);
		double clock = lua_tonumber(functionState, 2);
		creatureReference->newClockState(clock,functionName);
		
	}
	return 0;
}

int LuaCreature::l_deleteState(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		std::string stateName = lua_tostring(functionState, 1);
		creatureReference->deleteState(stateName);
	}
	return 0;
}

int LuaCreature::l_setCreatureMovementSpeed(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 1) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = lua_tonumber(functionState, 1);
		creatureReference->setMovementSpeed(movementSpeed);
	}
	return 0;
}

int LuaCreature::l_getCreatureMovementSpeed(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 0) {
		Creature* creatureReference = getCreatureReference(functionState);
		float movementSpeed = creatureReference->getMovementSpeed();
		lua_pushnumber(functionState, movementSpeed);
		return 1;
	}
	return 0;
}

int LuaCreature::l_creatureMoveTo(lua_State* functionState) {
	CHECK_ARGS;

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
	CHECK_ARGS;

	Creature* creatureReference = getCreatureReference(functionState);
	if (lua_gettop(functionState) == 0) {
		creatureReference->despawn();
	}
	return 0;
}
