#pragma once
#include "LuaGame.h"

class LuaActions : public LuaGame {
public:
	void init(Vec2i tileCoordinates);
protected:
	virtual void addFunctions(lua_State*);
private:
	static int l_moveOn(lua_State*);
	static int l_moveTo(lua_State*);
	static int l_interactMap(lua_State*);

	Vec2i m_tilePos;
};
