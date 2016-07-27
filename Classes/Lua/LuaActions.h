#pragma once
#include "LuaPersonal.h"

class LuaActions : public LuaPersonal {
protected:
	virtual void addFunctions(lua_State*);
private:
};
