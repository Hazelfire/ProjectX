#include "LuaPersonal.h"

class LuaItemUses : public LuaPersonal {
public:
	void init(std::string itemName);

protected:
	virtual void addFunctions(lua_State*);

	virtual std::string getScopeName() { return "ItemUses"; };
private:
	std::string m_itemName;
};