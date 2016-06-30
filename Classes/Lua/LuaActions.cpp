#include "LuaActions.h"
#include "Interactions.h"
#include "Arena.h"
#include "Inventory.h"
#include "Multiplayer/XClient.h"

int LuaActions::m_Playerx;
int LuaActions::m_Playery;

void LuaActions::init(int x, int y) {
	m_Playerx = x;
	m_Playery = y;
}

void LuaActions::addFunctions(lua_State* mainState) {

	LuaGame::addFunctions(mainState);

	//Interact
	NEW_TABLE_SIZE(3);

	NEW_ROW("moveOn", l_moveOn);
	NEW_ROW("moveTo", l_moveTo);
	NEW_ROW("interactMap", l_interactMap);

	NAME_TABLE("Interact");

	// sets the x anad y coordinates to the tiles as globals x and y. 
	// This goes against all my knowledge as a programmer but it is efficient and looks nice
	lua_pushinteger(mainState, m_Playerx);
	lua_setglobal(mainState, "x");

	lua_pushinteger(mainState, m_Playery);
	lua_setglobal(mainState, "y");

	int playerIndex = XClient::getPlayerIndex();
	
	luaW_push<LuaPlayerObject>(mainState, new LuaPlayerObject(playerIndex));
	lua_setglobal(mainState, "player");

	luaW_push<LuaInventoryObject>(mainState, new LuaInventoryObject(playerIndex));
	lua_setglobal(mainState, "inventory");
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
	Interact::moveOn(m_Playerx, m_Playery);
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
	else if (lua_gettop(functionState) == 0) {
		Interact::moveTo(m_Playerx, m_Playery);
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