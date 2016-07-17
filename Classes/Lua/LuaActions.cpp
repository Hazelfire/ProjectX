#include "LuaActions.h"
#include "Interactions.h"
#include "Arena.h"
#include "Inventory.h"
#include "Multiplayer/XClient.h"

void LuaActions::addFunctions(lua_State* mainState) {

	LuaPersonal::addFunctions(mainState);

	//Interact
	NEW_FUNCTION("moveOn", l_moveOn);
	NEW_FUNCTION("moveTo", l_moveTo);
	NEW_FUNCTION("interactMap", l_interactMap);
}

int LuaActions::l_moveOn(lua_State* functionState) {

	if(!assertArguments(functionState, "moveOn", {
				{ LUA_TTABLE },
				{ LUA_TTABLE, LUA_TNUMBER}
				})) return 0;

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		Interact::moveOn(position);
	}
	else if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);
		double speed = lua_tonumber(functionState, 2);
		Interact::moveOn(position, speed);
	}
	return 0;
}

int LuaActions::l_moveTo(lua_State* functionState) {

	if(!assertArguments(functionState, "moveTo", {
				{ LUA_TTABLE },
				{ LUA_TTABLE, LUA_TNUMBER },
				{ LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER }
				})) return 0;

	if (lua_gettop(functionState) == 1) { // if there is 2 arguments
		Vec2d position = toVector(functionState, 1);
		Interact::moveTo(position);
	}
	else if (lua_gettop(functionState) == 2) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 2);
		Interact::moveTo(position, distance);
	}
	else if (lua_gettop(functionState) == 3) {
		Vec2d position = toVector(functionState, 1);
		int distance = lua_tointeger(functionState, 2);
		double speed = lua_tonumber(functionState, 3);
		Interact::moveTo(position, distance, speed);
	}
	return 0;
}

int LuaActions::l_interactMap(lua_State* functionState) {

	if(!assertArguments(functionState, "interactMap", {
				{ LUA_TTABLE }
				})) return 0;

	if (lua_gettop(functionState) == 1) {
		Vec2d position = toVector(functionState, 1);
		Interact::InteractMap(position);
	}
	return 0;
}
