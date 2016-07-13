#include <string>
#include "lua.hpp"
#include <list>
#include "luawrapper.hpp"
#include "MyMath.h"

#define NEW_TABLE_SIZE(__SIZE__) lua_createtable(mainState, 0, __SIZE__);
#define NEW_ROW(__NAME__, __FUNC__) lua_pushstring(mainState, __NAME__); lua_pushcfunction(mainState, __FUNC__); lua_settable(mainState, -3);
#define NAME_TABLE(__NAME__) lua_setglobal(mainState, __NAME__);
#define NEW_FUNCTION(__NAME__,__FUNC__) lua_pushcfunction(mainState, __FUNC__); lua_setglobal(mainState, __NAME__);
#define ASSERT_ARGS(__FNAME__,__ARGUMENTS__) if(!assertArguments(functionState, __FNAME__, __ARGUMENTS__)){ return 0;}

class LuaInterpreter {
public:
	void run(std::string scriptFile, std::string extra = "");
	void run(std::list<std::string> scriptFiles, std::string extra = "");
	void call(std::string function);
	bool fulfills(std::list<std::string> script, std::string function);
	LuaInterpreter();
	~LuaInterpreter();
protected:
	virtual void addFunctions(lua_State*);
	virtual void start(lua_State*) {};
	void cleanup();
	std::list<std::string> m_callbacks;
	lua_State* m_mainState;

	// lua functions
	// Math
	static int l_randomInt(lua_State*);
	static int l_randomReal(lua_State*);

	// File
	static int l_readStringFromFile(lua_State*);

	// Debug
	static int l_debugLog(lua_State*);
	static int l_debugLogWarning(lua_State*);
	static int l_debugLogError(lua_State*);

	// various tools
	static std::string pickleTable(lua_State*, int index);
	static void unPickleTable(lua_State*, std::string);
	static std::string getType(lua_State*, int index);
	static std::string stringOfLuaType(int type);
	static bool assertArguments(lua_State*, std::string functionName, std::list < std::list<int> > overloads, bool memberMode = false);
	static void luaError(lua_State* ,std::string message);
	static void pushVector(lua_State* ,Vec2d vector);

	static Vec2d toVector(lua_State*, int index);
	// Vector operations
	static int l_addVector(lua_State*);
	static int l_subVector(lua_State*);
	static int l_mulVector(lua_State*);
	static int l_divVector(lua_State*);
	static int l_powVector(lua_State*);
	static int l_lenVector(lua_State*);
	static int l_eqVector(lua_State*);
};





