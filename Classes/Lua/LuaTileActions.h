#pragma once
#include "LuaActions.h"

class LuaTileActions : public LuaActions {
public:
	void init(Vec2i position);
protected:
	virtual void addFunctions(lua_State*);
	virtual std::string getScopeName() { return "TileActions"; };
private:
	static int l_collect(lua_State*);
	Vec2i m_tilePosition;
};