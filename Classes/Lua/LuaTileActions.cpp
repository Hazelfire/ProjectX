#include "LuaTileActions.h"

void LuaTileActions::init(Vec2i tilePos) {
	m_tilePosition = tilePos;
}

void LuaTileActions::addFunctions(lua_State* state) {
	LuaActions::addFunctions(state);
	
	// sets the x and y coordinates to the tiles as globals x and y. 
	// This goes against all my knowledge as a programmer but it is efficient and looks nice
	lua_pushinteger(state, m_tilePosition.x);
	lua_setglobal(state, "x");

	lua_pushinteger(state, m_tilePosition.y);
	lua_setglobal(state, "y");
}