#include "LuaTileActions.h"

void LuaTileActions::init(Vec2i tilePos) {
	m_tilePosition = tilePos;
}

void LuaTileActions::addFunctions(lua_State* state) {
	LuaActions::addFunctions(state);
	
	pushVector(state, m_tilePosition);
	lua_setglobal(state, "tilePos");
}
