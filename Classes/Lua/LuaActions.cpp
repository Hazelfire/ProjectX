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
	if (lua_gettop(functionState) == 2) { // if there is 2 arguments
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		Interact::moveOn(x, y);
	}
	else if (lua_gettop(functionState) == 3) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		double speed = lua_tonumber(functionState, 3);
		Interact::moveOn(x, y, speed);
	}
	return 0;
}

int LuaActions::l_moveTo(lua_State* functionState) {
	if (lua_gettop(functionState) == 2) { // if there is 2 arguments
		int x = lua_tointeger(functionState, 1);
		int y = lua_tonumber(functionState, 2);
		Interact::moveTo(x,y);
	}
	else if (lua_gettop(functionState) == 3) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		Interact::moveTo(x, y, distance);
	}
	else if (lua_gettop(functionState) == 4) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		int distance = lua_tointeger(functionState, 3);
		double speed = lua_tonumber(functionState, 4);
		Interact::moveTo(x, y, distance, speed);
	}
	return 0;
}

int LuaActions::l_interactMap(lua_State* functionState) {
	if (lua_gettop(functionState) == 2) {
		int x = lua_tointeger(functionState, 1);
		int y = lua_tointeger(functionState, 2);
		Interact::InteractMap(x,y);
	}
	return 0;
}