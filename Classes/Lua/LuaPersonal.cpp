#include "LuaPersonal.h"
#include "Multiplayer/XClient.h"

void LuaPersonal::addFunctions(lua_State* state) {

	// Add game functions first
	LuaGame::addFunctions(state);

	int playerIndex = XClient::getPlayerIndex();
	
	pushPlayer(state, playerIndex);
	lua_setglobal(state, "me");
}