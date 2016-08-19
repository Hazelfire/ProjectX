#include "LuaTileActions.h"
#include "Interactions.h"

void LuaTileActions::init(Vec2i tilePos) {
	m_tilePosition = tilePos;
}

void LuaTileActions::addFunctions(lua_State* state) {
	LuaActions::addFunctions(state);
	
	pushVector(state, m_tilePosition);
	lua_setglobal(state, "tilePos");

	pushCFunction(state,
		0,
		{ "collect",
		{ "Waits the given duration and gives the item while destroying the tile" },
		{ { { LUA_TSTRING, "itemName" }, { LUA_TNUMBER, "quantity" }, { LUA_TVECTOR, "tilePosition" } , { LUA_TNUMBER, "time" } } },
		0,
		l_collect });
	lua_setglobal(state, "collect");
}

int LuaTileActions::l_collect(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 4) {
		std::string itemName = lua_tostring(functionState, 1);
		int quantity = lua_tointeger(functionState, 2);
		Vec2i tilePos = toVector(functionState, 3);
		double time = lua_tonumber(functionState, 4);
		Interact::scheduleCollection(itemName, quantity, tilePos, time);
	}
}