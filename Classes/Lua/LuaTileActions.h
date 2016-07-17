#include "LuaActions.h"

class LuaTileActions : public LuaActions {
public:
	void init(Vec2i position);
protected:
	void addFunctions(lua_State*);
private:
	Vec2i m_tilePosition;
};