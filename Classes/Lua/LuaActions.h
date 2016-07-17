#pragma once
#include "LuaPersonal.h"

class LuaActions : public LuaPersonal {
protected:
	virtual void addFunctions(lua_State*);
private:
	static int l_moveOn(lua_State*);
	static int l_moveTo(lua_State*);
	static int l_interactMap(lua_State*);
};
