#pragma once
#include "LuaGame.h"

class LuaActions : public LuaGame {
public:
	static void init(int x, int y);
protected:
	virtual void addFunctions(lua_State*);
private:
	static int l_moveOn(lua_State*);
	static int l_moveTo(lua_State*);
	static int l_interactMap(lua_State*);
	static int m_Playerx;
	static int m_Playery;
};