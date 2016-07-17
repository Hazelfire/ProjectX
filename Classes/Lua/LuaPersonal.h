#pragma once
#include "LuaGame.h"

class LuaPersonal : public LuaGame {
protected:
	virtual void addFunctions(lua_State*);
};