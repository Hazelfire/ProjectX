#include <string>
#include "lua.hpp"
#include <list>
#include <vector>
#include "MyMath.h"

#define NEW_TABLE_SIZE(__SIZE__) lua_createtable(mainState, 0, __SIZE__);
#define NEW_ROW(__NAME__,__DOCS__, __OVERLOADS__,  __FUNC__ ) lua_pushstring(mainState, __NAME__); pushCFunction(mainState, 0, __NAME__, __DOCS__, __OVERLAODS__, __FUNC__); lua_settable(mainState, -3);
#define NAME_TABLE(__NAME__) lua_setglobal(mainState, __NAME__);
#define CHECK_ARGS if(!assertArguments(functionState)) return 0
#define GET_META_INT(__VAR__, __INDEX__) pushSelfMeta(functionState, __INDEX__); int __VAR__ = lua_tointeger(functionState, -1); lua_pop(functionState, 1)
#define GET_META_VEC(__VAR__, __INDEX__) pushSelfMeta(functionState, __INDEX__); Vec2d __VAR__ = toVector(functionState, -1); lua_pop(functionState, 1)
#define LUA_TPLAYER 9
#define LUA_TTILE 10
#define LUA_TVECTOR 11
#define LUA_TCREATURE 12

#define UP_SELF 1
#define UP_NAME 2
#define UP_DOCS 3
#define UP_OVERLOAD 4
#define UP_RETURNTYPE 5

typedef std::list<std::pair<int, std::string>> LuaOverload;
typedef std::list<LuaOverload> LuaOverloadList;

class LuaInterpreter {
public:
	std::string run(std::string scriptFile, std::string extra = "");
	std::string run(std::list<std::string> scriptFiles, std::string extra = "");
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

	// Debug
	static int l_debugLog(lua_State*);
	static int l_debugLogWarning(lua_State*);
	static int l_debugLogError(lua_State*);

	// table pickling (serialization)
	static std::string pickleTable(lua_State*, int index);
	static void unPickleTable(lua_State*, std::string);

	// special getType, supports my types
	static int getType(lua_State*, int index);
	
	// Returns a string representation of the type
	static std::string stringOfLuaType(int type);

	// Checks the given arguemnents against the function's overload
	static bool assertArguments(lua_State*);

	// Displays an error along with a traceback (if needed)
	static void luaError(lua_State* ,std::string message);


	struct MyCFunction {
		std::string functionName;
		std::string functionDocs;
		LuaOverloadList overloads;
		int returnType;
		lua_CFunction l_CFunction;
	};
	
	// Pushes a generic object with some common features
	static void pushObject(lua_State*, std::string objectName, std::list<MyCFunction> memberFunctions);
	static int l_objectFailIndex(lua_State* functionState);
	static void pushSelf(lua_State* functionState);
	static void pushSelfMeta(lua_State* functionState, std::string metaIndex);

	// Pushes a C function onto the stack, with a couple more features
	// Using 0 as a parent will make it static
	static void pushCFunction(lua_State*, int parent, MyCFunction function);

	// Pushes a vector onto the stack
	static void pushVector(lua_State* ,Vec2d vector);
	
	// String representation of a vector
	static std::string vectorToString(Vec2d vector);
	static Vec2d toVector(lua_State*, int index);
	// Vector operations
	static int l_addVector(lua_State*);
	static int l_subVector(lua_State*);
	static int l_mulVector(lua_State*);
	static int l_divVector(lua_State*);
	static int l_lenVector(lua_State*);
	static int l_eqVector(lua_State*);

	static int l_vecConstruct(lua_State*);

	// type checkers
	static int isNil(lua_State* state, int index) { return lua_isnil(state, index); };
	static int isBoolean(lua_State* state, int index) { return lua_isboolean(state, index); };
	static int isNumber(lua_State* state, int index) { return lua_isnumber(state, index); };
	static int isString(lua_State* state, int index) { return lua_isstring(state, index); };
	static int isTable(lua_State* state, int index) { return lua_istable(state, index); };
	static int isFunction(lua_State* state, int index) { return lua_isfunction(state, index); };

	static int isPlayer(lua_State* state, int index);
	static int isTile(lua_State* state, int index);
	static int isVector(lua_State* state, int index);
	static int isCreature(lua_State* state, int index);

	// type definitions
	static std::vector<std::pair<std::string, int(*)(lua_State*, int)>> registeredTypes;
};