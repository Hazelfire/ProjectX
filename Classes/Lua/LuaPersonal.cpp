#include "LuaPersonal.h"
#include "Multiplayer/XClient.h"

void LuaPersonal::addFunctions(lua_State* state) {

	// Add game functions first
	LuaGame::addFunctions(state);

	int playerIndex = XClient::getPlayerIndex();
	
	luaW_push<LuaPlayerObject>(state, new LuaPlayerObject(playerIndex));
	lua_setglobal(state, "player");

	luaW_push<LuaInventoryObject>(state, new LuaInventoryObject(playerIndex));
	lua_setglobal(state, "inventory");
}