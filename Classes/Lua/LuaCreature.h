#pragma once
#include "LuaGame.h"
class LuaCreature : public LuaGame{
public:
	void init(short int creatureGid);
protected:
	virtual void addFunctions(lua_State*);
	virtual void start(lua_State*);

	virtual std::string getScopeName() { return "Creature"; };

	static int l_getCreaturePosition(lua_State*);
	static int l_getCreatureRealPosition(lua_State*);
	static int l_creatureMoveOn(lua_State*);
	static int l_newProximityState(lua_State*);
	static int l_newUpdateState(lua_State*);
	static int l_newClockState(lua_State*);
	static int l_deleteState(lua_State*);
	static int l_setCreatureMovementSpeed(lua_State*);
	static int l_getCreatureMovementSpeed(lua_State*);
	static int l_creatureMoveTo(lua_State*);
	static int l_despawn(lua_State*);

	short int m_creatureGID;
};