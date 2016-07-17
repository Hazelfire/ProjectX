#include "LuaPersonal.h"

class LuaItemUses : public LuaPersonal {
public:
	void init(std::string itemName);

protected:
	virtual void addFunctions(lua_State*);
private:
	std::string m_itemName;
};