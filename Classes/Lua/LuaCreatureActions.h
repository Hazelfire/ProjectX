#pragma once
#include "LuaActions.h"

class LuaCreatureActions : public LuaActions {
public:
	void init(LuaCreatureObject luaCreature);
protected:
	virtual void addFunctions(lua_State*);
private:
	LuaCreatureObject m_luaCreature;
};