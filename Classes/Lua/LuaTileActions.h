#include "LuaActions.h"

class LuaTileActions : public LuaActions {
public:
	void init(Vec2i position);
protected:
	virtual void addFunctions(lua_State*);
	virtual std::string getScopeName() { return "TileActions"; };
private:
	Vec2i m_tilePosition;
};