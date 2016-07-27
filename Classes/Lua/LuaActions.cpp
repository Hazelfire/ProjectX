#include "LuaActions.h"
#include "Interactions.h"
#include "Arena.h"
#include "Inventory.h"
#include "Multiplayer/XClient.h"

void LuaActions::addFunctions(lua_State* mainState) {

	LuaPersonal::addFunctions(mainState);
}