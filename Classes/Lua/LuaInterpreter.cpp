#include "LuaInterpreter.h"
#include <cocos2d.h>
#include "Debug.h"
#include "Parser/StringOperations.h"
#include "MyUI/LuaTerminal.h"

std::string pickleScript = "----------------------------------------------\r\n-- Pickle.lua\r\n-- A table serialization utility for lua\r\n-- Steve Dekorte, http://www.dekorte.com, Apr 2000\r\n-- Freeware\r\n----------------------------------------------\r\n\r\nfunction _pickle(t)\r\n  return Pickle:clone():pickle_(t)\r\nend\r\n\r\nPickle = {\r\n  clone = function (t) local nt={}; for i, v in pairs(t) do nt[i]=v end return nt end \r\n}\r\n\r\nfunction Pickle:pickle_(root)\r\n  if type(root) ~= \"table\" then \r\n    error(\"can only pickle tables, not \".. type(root)..\"s\")\r\n  end\r\n  self._tableToRef = {}\r\n  self._refToTable = {}\r\n  local savecount = 0\r\n  self:ref_(root)\r\n  local s = \"\"\r\n\r\n  while #self._refToTable > savecount do\r\n    savecount = savecount + 1\r\n    local t = self._refToTable[savecount]\r\n    s = s..\"{\\n\"\r\n    for i, v in pairs(t) do\r\n        s = string.format(\"%s[%s]=%s,\\n\", s, self:value_(i), self:value_(v))\r\n    end\r\n    s = s..\"},\\n\"\r\n  end\r\n\r\n  return string.format(\"{%s}\", s)\r\nend\r\n\r\nfunction Pickle:value_(v)\r\n  local vtype = type(v)\r\n  if     vtype == \"string\" then return string.format(\"%q\", v)\r\n  elseif vtype == \"number\" then return v\r\n  elseif vtype == \"boolean\" then return tostring(v)\r\n  elseif vtype == \"table\" then return \"{\"..self:ref_(v)..\"}\"\r\n  else --error(\"pickle a \"..type(v)..\" is not supported\")\r\n  end  \r\nend\r\n\r\nfunction Pickle:ref_(t)\r\n  local ref = self._tableToRef[t]\r\n  if not ref then \r\n    if t == self then error(\"can't pickle the pickle class\") end\r\n    table.insert(self._refToTable, t)\r\n    ref = #self._refToTable\r\n    self._tableToRef[t] = ref\r\n  end\r\n  return ref\r\nend\r\n\r\n----------------------------------------------\r\n-- unpickle\r\n----------------------------------------------\r\n\r\nfunction _unpickle(s)\r\n  if type(s) ~= \"string\" then\r\n    error(\"can't unpickle a \"..type(s)..\", only strings\")\r\n  end\r\n  local gentables = load(\"return \"..s)\r\n  local tables = gentables()\r\n  \r\n  for tnum = 1, #tables do\r\n    local t = tables[tnum]\r\n    local tcopy = {}; for i, v in pairs(t) do tcopy[i] = v end\r\n    for i, v in pairs(tcopy) do\r\n      local ni, nv\r\n      if type(i) == \"table\" then ni = tables[i[1]] else ni = i end\r\n      if type(v) == \"table\" then nv = tables[v[1]] else nv = v end\r\n      t[i] = nil\r\n      t[ni] = nv\r\n    end\r\n  end\r\n  return tables[1]\r\nend";


std::vector<std::pair<std::string, int(*)(lua_State*, int)>> LuaInterpreter::registeredTypes = {
	{ "nil", isNil},
	{ "boolean", isBoolean},
	{ "lightuserdata", nullptr},
	{ "number", isNumber },
	{ "string", isString },
	{ "table", isTable },
	{ "function", isFunction},
	{ "userdata", nullptr },
	{ "thread", nullptr },
	{ "player", isPlayer},
	{ "tile", isTile },
	{ "vector", isVector },
	{ "creature", isCreature}
};

std::string LuaInterpreter::run(std::string scriptFile, std::string extra) {


	
	// add if you wish, standard library for lua, I am not sure how safe this is cross-platform
	luaL_openlibs(m_mainState);

	// adding pickle utilities
	luaL_dostring(m_mainState, pickleScript.c_str());

	addFunctions(m_mainState); // Calls children and adds all functions

	// Loading the string into lua
	if (!scriptFile.empty()) {
		int error = luaL_dofile(m_mainState, scriptFile.c_str());
		if (error) {
			luaError(m_mainState, lua_tostring(m_mainState, -1));
			lua_pop(m_mainState, 1);
		}
	}

	int error = luaL_dostring(m_mainState, extra.c_str());
	if(error){
		luaError(m_mainState, lua_tostring(m_mainState, -1));
		lua_pop(m_mainState, 1);
	}

	return serializeStack(m_mainState);

}

std::string LuaInterpreter::run(std::list<std::string> scripts, std::string extra){

	// add if you wish, standard library for lua, I am not sure how safe this is cross-platform
	luaL_openlibs(m_mainState);

	// adding pickle utilities
	luaL_dostring(m_mainState, pickleScript.c_str());

	addFunctions(m_mainState); // Calls children and adds all functions
	for (auto script : scripts){
		int error = luaL_dofile(m_mainState, script.c_str());
		if(error){
			luaError(m_mainState, lua_tostring(m_mainState, -1));
			lua_pop(m_mainState, 1);
		}
	}
	int error = luaL_dostring(m_mainState, extra.c_str());
	if(error){
		luaError(m_mainState, lua_tostring(m_mainState, -1));
		lua_pop(m_mainState, 1);
	}
	
	return serializeStack(m_mainState);

}
std::string LuaInterpreter::serializeStack(lua_State* state) {

	if (lua_gettop(state) == 0) {
		return "";
	}
	else {
		std::string returnValues;
		while(lua_gettop(state) != 0){
			
			returnValues += toString(state, -1);
			lua_pop(state, 1);
			if (0 != lua_gettop(state))
				returnValues += ", ";
		}
		return returnValues;
	}
}

void LuaInterpreter::call(std::string function) {
	lua_getglobal(m_mainState,function.c_str());

	int error = lua_pcall(m_mainState, 0, 0, 0);
	if (error) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		luaError(m_mainState,"Failed calling function " + function + " with error: " + lua_tostring(m_mainState, -1));
#endif
		lua_pop(m_mainState, 1);
	}
}

bool LuaInterpreter::fulfills(std::list<std::string> scriptFiles, std::string function) {
	lua_State* mainState = luaL_newstate();

	luaL_dostring(mainState, pickleScript.c_str());
	
	

	// add if you wish, standard library for lua, I am not sure how safe this is cross-platform
	luaL_openlibs(mainState);

	luaL_loadstring(mainState, function.c_str());
		
	addFunctions(mainState); // Calls children and adds all functions
							   // Calls the script
	for(auto scriptFile : scriptFiles){
		int error = luaL_dofile(mainState, scriptFile.c_str());
		if(error){
			luaError(mainState, "Failed loading script: " + scriptFile + " with error: " + lua_tostring(m_mainState, -1) + " When checking if " + function +  " fulfills" );
			lua_pop(mainState, 1);
		 }
	}	int error = lua_pcall(mainState, 0, 1, 0);



	if (error) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		luaError(mainState, "Error furfilling requirement: " + function + " error is: " + lua_tostring(mainState, -1));
#endif
		lua_pop(mainState, 1);
	}
	else if (lua_isboolean(mainState, -1)) {
		return lua_toboolean(mainState, -1);
	}
	else {
		luaError(mainState, "Expression: " + function + " did not return a boolean, cannot be used as requirement");
	}
	return false;
}

void LuaInterpreter::addFunctions(lua_State* mainState) {

	pushCFunction(mainState,
		0,
		{ "log",
		"Logs a message to the log file and prints it to the console",
		{ { { LUA_TSTRING, "message" } } },
		0,
		l_debugLog });
	lua_setglobal(mainState, "log");

	pushCFunction(mainState,
		0,
		{ "logWarning",
		"Logs a warning to the log file and prints it to the console",
		{ { { LUA_TSTRING, "warning" } } },
		0,
		l_debugLogWarning });
	lua_setglobal(mainState, "logWarning");

	pushCFunction(mainState,
		0,
		{ "logError",
		"Logs an error to the log file and prints it to the console",
		{ { { LUA_TSTRING, "error" } } },
		0,
		l_debugLogError });
	lua_setglobal(mainState, "logError");

	pushCFunction(mainState,
		0,
		{ "type",
		"Returns the type of the item given",
		{ { { LUA_TANYTHING, "item" } } },
		LUA_TSTRING,
		l_type });

	lua_setglobal(mainState, "type");

	// Vectors
	pushCFunction(mainState, 
		0,
		{ "vector",
		"Returns a vector newly constructed vector",
		{ { },
		{ { LUA_TNUMBER, "x" }, { LUA_TNUMBER, "y" } } },
		LUA_TVECTOR,
		l_vecConstruct });
	lua_setglobal(mainState, "vector");
}

LuaInterpreter::~LuaInterpreter() {
	lua_close(m_mainState);
}

LuaInterpreter::LuaInterpreter() {
	m_mainState = luaL_newstate();
}

void LuaInterpreter::cleanup() {
	lua_close(m_mainState);
}

int LuaInterpreter::l_debugLog(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 1) {
		
		std::string message = lua_tostring(functionState, 1);
		Debugger::log(message, DEBUG_LUA);
	}
	return 0;
}

int LuaInterpreter::l_debugLogWarning(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 1) {
		
		std::string message = lua_tostring(functionState, 1);
		Debugger::logWarning(message, DEBUG_LUA);
		
	}
	return 0;
}

int LuaInterpreter::l_debugLogError(lua_State* functionState) {
	CHECK_ARGS;

	if (lua_gettop(functionState) == 1) {
		
		std::string message = lua_tostring(functionState, 1);
		Debugger::logError(message, DEBUG_LUA);
	}
	return 0;
}

std::string LuaInterpreter::pickleTable(lua_State* state, int index) {
	
	lua_getglobal(state, "_pickle");
	lua_pushvalue(state, index);
	int error = lua_pcall(state, 1, 1, 0);

	if (error) {
		std::string errorMessage = lua_tostring(state, -1);
		luaError(state, "Error pickling table: " + errorMessage);
		lua_pop(state, 1);
	}
	else {
		std::string pickledTable = lua_tostring(state, -1);
		return pickledTable;
	}
	
	return "{}";

}

void LuaInterpreter::unPickleTable(lua_State* state, std::string pickledTable) {

	lua_getglobal(state, "_unpickle");
	lua_pushstring(state, pickledTable.c_str());
	int error = lua_pcall(state, 1, 1, 0);
	if (error) {
		std::string errorMessage = lua_tostring(state, -1);
		luaError(state, "Error unpickling table: " + errorMessage);
		lua_pop(state, 1);
	}
}

bool LuaInterpreter::assertArguments(lua_State* state) {
	
	// Lets quickly check if this function is being called in "Help mode"
	//  i.e. has the help object as the first argument
	if (lua_type(state, 1) == LUA_TTABLE) {
		lua_getmetatable(state, 1);
		lua_pushstring(state, "type");
		lua_gettable(state, -2);
		if (lua_isstring(state, -1)) {
			std::string type = lua_tostring(state, -1);
			if (type == "help") {
				// It is a help object! sweet! Lets print the docs to the console
				std::string docs = constructHelp(state);
				if (LuaTerminal::getInstance())
					LuaTerminal::getInstance()->print(docs);
				lua_pop(state, 2);
				return false;
			}
		}
		lua_pop(state, 2);
	}

	std::string functionName = lua_tostring(state, lua_upvalueindex(UP_NAME));

	LuaOverloadList overloads = getOverloads(state);

	for (auto currentOverload : overloads) {

		// Magic number, -1 means that there can be an unlimited amount of arguments
		if (!currentOverload.empty() && currentOverload.front().first == -1) return true;


		if (currentOverload.size() != lua_gettop(state)) continue; // if the number of paramaters do not match, it is definitely not a valid overload, continue

		bool overloadValid = true;
		int argumentIndex =  1;
		for (auto currentParameter : currentOverload) {

			auto type = registeredTypes[currentParameter.first];
			// first is name, second is "is function", returns if it is one or not
			// We are more interested in the second right now

			// If the type does not convert, this overload does not work
			if (!type.second(state, argumentIndex)) {
				overloadValid = false;
				break;
			}

			argumentIndex++;
		}
		if (overloadValid) // if all parameters match, give it an all go!
			return true;
	}

	// no overloads match, print a helpful message
	std::string errorMessage = "No instance of " + functionName + " matches the argument list: (";

	// printing used overload
	for (int i = 1; i <= lua_gettop(state); i++) {
		errorMessage.append(stringOfLuaType(getType(state, i)));
		if (i != lua_gettop(state))
			errorMessage.append(", ");
	}

	// printing valid overloads
	errorMessage.append(")\n\toverloads of " + functionName + ":");
	for (auto overload : overloads) {
		errorMessage.append( "\n\t"+functionName + "(");
		int parameterIndex = 0;
		for (auto parameter : overload) {

			errorMessage.append(stringOfLuaType(parameter.first));
			errorMessage.append(" " + parameter.second); // Second has a display name
			if (parameterIndex != overload.size() - 1) // if we are not on our last parameter
				errorMessage.append(", ");

			parameterIndex++;
		}
		errorMessage.append(")");
	}

	luaError(state, errorMessage);
	return false;
}

int LuaInterpreter::getType(lua_State* state, int index) {

	// Here is what lua thinks it is, but I have invented a few types
	// So it may not be
	int initialType = lua_type(state, index);

	// All the objects are stored as tables, lets check if it is an object
	if (initialType == LUA_TTABLE) {

		lua_getmetatable(state, index);

		lua_pushstring(state, "type");
		lua_gettable(state, -2);
		std::string type;
		if (lua_isstring(state, -1))
			type = lua_tostring(state, -1);
		else
			return LUA_TTABLE; // table does not have a type value in it's metatable
							   // It is a table, not one of my objects

		lua_pop(state, 2);

		// Got the type, lets check if it is any of out objects

		if (type == "player") {
			return LUA_TPLAYER;
		}
		else if (type == "tile") {
			return LUA_TTILE;
		}
		else if (type == "vector") {
			return LUA_TVECTOR;
		}
		else if (type == "creature") {
			return LUA_TCREATURE;
		}
		else {
			// I don't know what this person is up to, but
			// this is not a standard type, must be a table
			return LUA_TTABLE;
		}

	}

	// Otherwise, lua was right, and it was the type they
	// thought it was

	return initialType;
}

std::string LuaInterpreter::stringOfLuaType(int type) {
	if (type == LUA_TNIL)
		return "nil";
	else if (type == LUA_TBOOLEAN)
		return "boolean";
	else if (type == LUA_TLIGHTUSERDATA)
		return "lightuserdata";
	else if (type == LUA_TNUMBER)
		return "number";
	else if (type == LUA_TSTRING)
		return "string";
	else if (type == LUA_TTABLE)
		return "table";
	else if (type == LUA_TFUNCTION)
		return "function";
	else if (type == LUA_TUSERDATA)
		return "userdata";
	else if (type == LUA_TTHREAD)
		return "thread";
	else if (type == LUA_TPLAYER)
		return "player";
	else if (type == LUA_TTILE)
		return "tile";
	else if (type == LUA_TCREATURE)
		return "creature";
	else if (type == LUA_TVECTOR)
		return "vector";
	else if (type == LUA_TEVERYTHING)
		return "everything";
	else if (type == LUA_TANYTHING)
		return "anything";
	else
		return "none";
}

void LuaInterpreter::pushObject(lua_State* state, std::string objectName, std::list<MyCFunction> memberFunctions) {

	lua_createtable(state, 0, memberFunctions.size());

	for (auto member : memberFunctions) {
		lua_pushstring(state, member.functionName.c_str());
		pushCFunction(state, -2 , member);
		lua_settable(state, -3);
	}

	// meta table
	lua_createtable(state, 0, 1);

	lua_pushstring(state, "__index");
	
	lua_pushvalue(state, -3);
	lua_pushcclosure(state, l_objectFailIndex, 1);

	lua_settable(state, -3);

	lua_pushstring(state, "type");
	lua_pushstring(state, objectName.c_str());
	lua_settable(state, -3);


	lua_setmetatable(state, -2);
}


void LuaInterpreter::pushCFunction(lua_State* state, int parent, MyCFunction function) {
	if (parent == 0) {
		lua_pushnil(state);
	}
	else {
		lua_pushvalue(state, parent);
	}

	lua_pushstring(state, function.functionName.c_str());
	lua_pushstring(state, function.functionDocs.c_str());
	
	// overload table
	lua_createtable(state, 0, function.overloads.size());

	// For every overload
	int overloadIndex = 1;
	for (auto overload : function.overloads) {
		lua_pushinteger(state, overloadIndex);
		lua_createtable(state, 0, overload.size());

		// For every paramater in the overload
		int parameterIndex = 1;
		for (auto parameter : overload) {
			lua_pushinteger(state, parameterIndex);

			// Pair containing paramater type and name for viewing
			lua_createtable(state, 0, 2);
			
			lua_pushinteger(state, 1);
			lua_pushinteger(state, parameter.first);
			lua_settable(state, -3);

			lua_pushinteger(state, 2);
			lua_pushstring(state, parameter.second.c_str());
			lua_settable(state, -3);

			lua_settable(state, -3);
			parameterIndex++;
		}

		lua_settable(state, -3);
		overloadIndex++;
	}

	lua_pushinteger(state, function.returnType);

	lua_pushcclosure(state, function.l_CFunction, 5);
}

int LuaInterpreter::l_objectFailIndex(lua_State* functionState) {
	std::string message;

	lua_getmetatable(functionState, 1);
	lua_pushstring(functionState, "type");
	lua_gettable(functionState, -2);
	
	std::string objectType = lua_tostring(functionState, -1);
	lua_pop(functionState, 1);

	std::string attemptedIndex = lua_tostring(functionState, 2);

	message += "No such member function " + attemptedIndex + " in object " + objectType;
	
	luaError(functionState, message);
	return 0;
}

void LuaInterpreter::pushSelf(lua_State* functionState) {
	lua_pushvalue(functionState, lua_upvalueindex(1));
}

void LuaInterpreter::pushSelfMeta(lua_State* functionState, std::string metaIndex) {
	pushSelf(functionState);
	lua_getmetatable(functionState, -1);

	lua_pushstring(functionState, metaIndex.c_str());
	lua_gettable(functionState, -2);
	
	// Take off the metatable and self
	lua_remove(functionState, -2);
	lua_remove(functionState, -2);
}

void LuaInterpreter::luaError(lua_State* state, std::string message) {
	std::string errorMessage;
	errorMessage.append(message);

	lua_getglobal(state, "debug");
	lua_getfield(state, -1, "traceback");
	lua_remove(state, -2);

	lua_pcall(state, 0, 1, 0);
	std::string traceback = lua_tostring(state, -1);
	lua_pop(state, 1);
	if (traceback != "stack traceback:")
		errorMessage.append("\n\t"+traceback);

	Debugger::logError(errorMessage, DEBUG_LUA);
}


void LuaInterpreter::pushVector(lua_State* state, Vec2d vector){
	pushObject(state, "vector", {
		{ "toString",
		"Returns a string representation of the vector",
		{ { } },
		LUA_TSTRING,
		l_vectorToString},

		{ "dot",
		"Returns the dot product of itself and it's argument",
		{ { { LUA_TVECTOR, "other" } } },
		LUA_TNUMBER,
		l_vectorDot},

		{ "length",
		"Returns the length of the vector",
		{ { } },
		LUA_TNUMBER,
		l_vectorLength},

		{ "distance",
		"Returns the distance between this vector and the argument",
		{ { { LUA_TVECTOR, "vector" } } },
		LUA_TNUMBER,
		l_vectorDistance},
	
		{ "argument",
		"Returns the argument of the vector",
		{ { } },
		LUA_TNUMBER,
		l_vectorAbsAngle},

		{ "normalized",
		"Returns the vector normalized",
		{ {} },
		LUA_TNUMBER,
		l_vectorNormalize}
	});

	lua_pushstring(state, "x");
	lua_pushnumber(state, vector.x);
	lua_settable(state, -3);

	lua_pushstring(state, "y");
	lua_pushnumber(state, vector.y);
	lua_settable(state, -3);
	
	// Vector meta-methods
	lua_getmetatable(state, -1);

	lua_pushstring(state, "__add");
	lua_pushcfunction(state, l_addVector);
	lua_settable(state, -3);

	lua_pushstring(state, "__sub");
	lua_pushcfunction(state, l_subVector);
	lua_settable(state, -3);

	lua_pushstring(state, "__mul");
	lua_pushcfunction(state, l_mulVector);
	lua_settable(state, -3);

	lua_pushstring(state, "__div");
	lua_pushcfunction(state, l_divVector);
	lua_settable(state, -3);
	
	lua_pushstring(state, "__len");
	lua_pushcfunction(state, l_lenVector);
	lua_settable(state, -3);

	lua_pushstring(state, "__eq");
	lua_pushcfunction(state, l_eqVector);
	lua_settable(state, -3);

	lua_setmetatable(state, -2);
}
Vec2d LuaInterpreter::toVector(lua_State* state, int index){
	index = index > 0 ? index : lua_gettop(state) + index + 1;
	lua_getmetatable(state, index);
	lua_pushstring(state, "type");
	lua_getmetatable(state, -2);
	if (lua_isstring(state, -1)) {
		lua_pop(state, 2);
		Vec2d re;

		lua_pushstring(state, "x");
		lua_gettable(state, index);
		re.x = lua_tonumber(state, -1);
		lua_pop(state, 1);

		lua_pushstring(state, "y");
		lua_gettable(state, index);
		re.y = lua_tonumber(state, -1);
		lua_pop(state, 1);
		return re;
	}
	lua_pop(state, 2);
	return Vec2d();
}
int LuaInterpreter::l_addVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);
	Vec2d vec2 = toVector(state, 2);

	pushVector(state, vec1 + vec2);
	return 1;

}
int LuaInterpreter::l_subVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);
	Vec2d vec2 = toVector(state, 2);

	pushVector(state, vec1 - vec2);
	return 1;	
}
int LuaInterpreter::l_mulVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);
	double double2 = lua_tonumber(state, 2);

	pushVector(state, vec1 * double2);

	return 1;
}
int LuaInterpreter::l_divVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);
	double double2 = lua_tonumber(state, 2);

	pushVector(state, vec1 / double2);

	return 1;
}
int LuaInterpreter::l_lenVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);

	lua_pushnumber(state, vec1.length());
	
	return 1;
}
int LuaInterpreter::l_eqVector(lua_State* state){
	Vec2d vec1 = toVector(state, 1);
	Vec2d vec2 = toVector(state, 2);

	lua_pushboolean(state, vec1 == vec2);
	
	return 1;
}

int LuaInterpreter::l_vectorToString(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, -1);
	std::string stringRepresentation = vectorToString(vector);

	lua_pushstring(functionState, stringRepresentation.c_str());
	return 1;
}

int LuaInterpreter::l_vectorDot(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, -1);
	lua_pop(functionState, 1);

	Vec2d other = toVector(functionState, 1);
	double result = vector.dot(other);
	lua_pushnumber(functionState, result);
	return 1;
}

int LuaInterpreter::l_vectorAbsAngle(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, -1);
	lua_pop(functionState, 1);

	double angle = vector.absoluteAngle();
	lua_pushnumber(functionState, angle);
	return 1;
}

int LuaInterpreter::l_vectorLength(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, -1);
	lua_pop(functionState, 1);

	double length = vector.length();
	lua_pushnumber(functionState, length);
	return 1;
}

int LuaInterpreter::l_vectorDistance(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, - 1);
	lua_pop(functionState, 1);

	Vec2d other = toVector(functionState, 1);
	double distance = vector.distance(other);
	lua_pushnumber(functionState, distance);
	return 1;
}

int LuaInterpreter::l_vectorNormalize(lua_State* functionState) {
	CHECK_ARGS;
	pushSelf(functionState);
	Vec2d vector = toVector(functionState, -1);
	lua_pop(functionState, 1);

	vector.normalize();
	pushVector(functionState, vector);
	return 1;
}

int LuaInterpreter::l_vecConstruct(lua_State* functionState){
	CHECK_ARGS;
	
	if(lua_gettop(functionState) == 0){
		pushVector(functionState, Vec2d(0,0));
	}
	else if (lua_gettop(functionState) == 2){
		int x = lua_tonumber(functionState, 1);
		int y = lua_tonumber(functionState, 2);
		pushVector(functionState, Vec2d(x,y));
	}

	return 1;
}

std::string LuaInterpreter::vectorToString(Vec2d vector) {
	std::string stringRepresentation = "(" + StringOps::to_string(vector.x) + ", " + StringOps::to_string(vector.y) + ")";
	return stringRepresentation;
}

int LuaInterpreter::isPlayer(lua_State* state, int index) {
	lua_getmetatable(state, index);
	lua_pushstring(state, "type");
	lua_gettable(state, -2);

	if (lua_isstring(state, -1)) {
		std::string type = lua_tostring(state, -1);
		if (type == "player") {
			lua_pop(state, 2);
			return 1;
		}
	}
	lua_pop(state, 2);
	return 0;
	
}
int LuaInterpreter::isTile(lua_State* state, int index) {
	lua_getmetatable(state, index);
	lua_pushstring(state, "type");
	lua_gettable(state, -2);

	if (lua_isstring(state, -1)) {
		std::string type = lua_tostring(state, -1);
		if (type == "tile") {
			lua_pop(state, 2);
			return 1;
		}
	}
	lua_pop(state, 2);
	return 0;
}
int LuaInterpreter::isVector(lua_State* state, int index) {

	index = index < 0 ? index + lua_gettop(state) + 1 : index;
	lua_getmetatable(state, index);
	lua_pushstring(state, "type");
	lua_gettable(state, -2);

	if (lua_isstring(state, -1)) {
		std::string type = lua_tostring(state, -1);
		if (type == "vector") {
			lua_pop(state, 2);
			return 1;
		}
	}
	lua_pop(state, 2);
	return 0;
}

int LuaInterpreter::isCreature(lua_State* state, int index) {
	lua_getmetatable(state, index);
	lua_pushstring(state, "type");
	lua_gettable(state, -2);

	if (lua_isstring(state, -1)) {
		std::string type = lua_tostring(state, -1);
		if (type == "creature") {
			lua_pop(state, 2);
			return 1;
		}
	}
	lua_pop(state, 2);
	return 0;
}

void LuaInterpreter::pushHelp(lua_State* functionState) {
	pushObject(functionState, "help", {});
}



std::string LuaInterpreter::constructHelp(lua_State* functionState){
	std::string functionName = lua_tostring(functionState, lua_upvalueindex(UP_NAME));
	if (lua_isnil(functionState, lua_upvalueindex(UP_SELF))) {
		
	}
	std::string helpMessage = lua_tostring(functionState, lua_upvalueindex(UP_DOCS));
	helpMessage += "\nOverloads:";
	LuaOverloadList overloads = getOverloads(functionState);

	for (auto currentOverload : overloads) {
		helpMessage += "\n    " + stringOfLuaType(lua_tonumber(functionState, lua_upvalueindex(UP_RETURNTYPE)));
		helpMessage += " " + std::string(lua_tostring(functionState, lua_upvalueindex(UP_NAME)));
		helpMessage += "(";

		int paramaterIndex = 1;
		for (auto currentParamater : currentOverload) {
			helpMessage += stringOfLuaType(currentParamater.first);
			helpMessage += " " + currentParamater.second;

			if (paramaterIndex != currentOverload.size()) {
				helpMessage += ", ";
			}
			paramaterIndex++;
		}

		helpMessage += ")";
	}
	return helpMessage;
}

LuaOverloadList LuaInterpreter::getOverloads(lua_State* state) {
	LuaOverloadList overloads;
	lua_pushnil(state);
	while (lua_next(state, lua_upvalueindex(UP_OVERLOAD)) != 0) {
		LuaOverload currentOverload;

		int arrayIndex = 1;
		lua_pushnumber(state, arrayIndex);
		lua_gettable(state, -2);
		while (!lua_isnil(state, -1)) {

			std::pair<int, std::string> parameter;

			// Get paramater type
			// type = paramater[1]
			lua_pushinteger(state, 1);
			lua_gettable(state, -2);
			parameter.first = lua_tointeger(state, -1);
			lua_pop(state, 1);

			// Get paramater name
			// name = paramater[2]
			lua_pushinteger(state, 2);
			lua_gettable(state, -2);
			parameter.second = lua_tostring(state, -1);
			lua_pop(state, 1);

			currentOverload.push_back(parameter);
			lua_pop(state, 1);

			arrayIndex++;
			lua_pushinteger(state, arrayIndex);
			lua_gettable(state, -2);
		}

		overloads.push_front(currentOverload);
		lua_pop(state, 2);
	}

	return overloads;
}

int LuaInterpreter::l_type(lua_State* functionState) {
	CHECK_ARGS;
	std::string type = stringOfLuaType(getType(functionState, 1));
	lua_pushstring(functionState, type.c_str());
	return 1;
}

std::string LuaInterpreter::toString(lua_State* functionState, int index) {
	if (index < 0) {
		index = lua_gettop(functionState) + index + 1;
	}

	std::string stringRep;
	if (lua_isstring(functionState, index))
		stringRep = lua_tostring(functionState, index);
	else if (lua_istable(functionState, index)) {
		// Might be one of my objects, lets check.
		lua_getmetatable(functionState, index);
		lua_pushstring(functionState, "type");
		lua_gettable(functionState, -2);
		if (lua_isstring(functionState, -1)) {
			// It IS!!! Lets call it's toString function
			
			lua_getfield(functionState, index, "toString");
			lua_pcall(functionState, 0, LUA_MULTRET, 0);
			
			stringRep = lua_tostring(functionState, -1);
			lua_pop(functionState, 1);
		}
		else {
			// Pardon me, I was wrong. Just pickle the table and show them
			stringRep = pickleTable(functionState, index);
		}
			lua_pop(functionState, 2);
		}
	else
		stringRep = stringOfLuaType(getType(functionState, index));
	return stringRep;
}
