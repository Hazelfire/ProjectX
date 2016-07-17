#include "LuaItemUses.h"

void LuaItemUses::init(std::string itemName) {
	m_itemName = itemName;
}

void LuaItemUses::addFunctions(lua_State* state) {
	LuaPersonal::addFunctions(state);

	lua_pushstring(state, m_itemName.c_str());
	lua_setglobal(state, "itemName");
}