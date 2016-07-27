#include "LuaCreatureActions.h"

void LuaCreatureActions::init(LuaCreatureObject luaCreature) {
	m_luaCreature = luaCreature;
}

void LuaCreatureActions::addFunctions(lua_State* mainState) {
	LuaActions::addFunctions(mainState);

	pushCreature(mainState, m_luaCreature);
	lua_setglobal(mainState, "creature");
}