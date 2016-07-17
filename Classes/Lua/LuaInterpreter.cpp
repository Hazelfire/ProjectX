#include "LuaInterpreter.h"
#include <CCFileUtils.h>
#include "Debug.h"
#include "Parser/StringOperations.h"

std::string pickleScript = "----------------------------------------------\r\n-- Pickle.lua\r\n-- A table serialization utility for lua\r\n-- Steve Dekorte, http://www.dekorte.com, Apr 2000\r\n-- Freeware\r\n----------------------------------------------\r\n\r\nfunction _pickle(t)\r\n  return Pickle:clone():pickle_(t)\r\nend\r\n\r\nPickle = {\r\n  clone = function (t) local nt={}; for i, v in pairs(t) do nt[i]=v end return nt end \r\n}\r\n\r\nfunction Pickle:pickle_(root)\r\n  if type(root) ~= \"table\" then \r\n    error(\"can only pickle tables, not \".. type(root)..\"s\")\r\n  end\r\n  self._tableToRef = {}\r\n  self._refToTable = {}\r\n  local savecount = 0\r\n  self:ref_(root)\r\n  local s = \"\"\r\n\r\n  while #self._refToTable > savecount do\r\n    savecount = savecount + 1\r\n    local t = self._refToTable[savecount]\r\n    s = s..\"{\\n\"\r\n    for i, v in pairs(t) do\r\n        s = string.format(\"%s[%s]=%s,\\n\", s, self:value_(i), self:value_(v))\r\n    end\r\n    s = s..\"},\\n\"\r\n  end\r\n\r\n  return string.format(\"{%s}\", s)\r\nend\r\n\r\nfunction Pickle:value_(v)\r\n  local vtype = type(v)\r\n  if     vtype == \"string\" then return string.format(\"%q\", v)\r\n  elseif vtype == \"number\" then return v\r\n  elseif vtype == \"boolean\" then return tostring(v)\r\n  elseif vtype == \"table\" then return \"{\"..self:ref_(v)..\"}\"\r\n  else --error(\"pickle a \"..type(v)..\" is not supported\")\r\n  end  \r\nend\r\n\r\nfunction Pickle:ref_(t)\r\n  local ref = self._tableToRef[t]\r\n  if not ref then \r\n    if t == self then error(\"can't pickle the pickle class\") end\r\n    table.insert(self._refToTable, t)\r\n    ref = #self._refToTable\r\n    self._tableToRef[t] = ref\r\n  end\r\n  return ref\r\nend\r\n\r\n----------------------------------------------\r\n-- unpickle\r\n----------------------------------------------\r\n\r\nfunction _unpickle(s)\r\n  if type(s) ~= \"string\" then\r\n    error(\"can't unpickle a \"..type(s)..\", only strings\")\r\n  end\r\n  local gentables = load(\"return \"..s)\r\n  local tables = gentables()\r\n  \r\n  for tnum = 1, #tables do\r\n    local t = tables[tnum]\r\n    local tcopy = {}; for i, v in pairs(t) do tcopy[i] = v end\r\n    for i, v in pairs(tcopy) do\r\n      local ni, nv\r\n      if type(i) == \"table\" then ni = tables[i[1]] else ni = i end\r\n      if type(v) == \"table\" then nv = tables[v[1]] else nv = v end\r\n      t[i] = nil\r\n      t[ni] = nv\r\n    end\r\n  end\r\n  return tables[1]\r\nend";


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

	if (lua_gettop(m_mainState) == 0) {
		return "";
	}
	else {
		std::string returnValues;
		for (int argumentIndex = 1; argumentIndex <= lua_gettop(m_mainState); argumentIndex++) {
			
			if (lua_isstring(m_mainState, argumentIndex)) {
				returnValues += lua_tostring(m_mainState, argumentIndex);
			}
			else if (lua_istable(m_mainState, argumentIndex)) {
				returnValues += pickleTable(m_mainState, argumentIndex);
			}
			else {
				returnValues += getType(m_mainState, argumentIndex);
			}

			if (argumentIndex != lua_gettop(m_mainState))
				returnValues += ", ";
		}
		return returnValues;
	}

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
	
	if (lua_gettop(m_mainState) == 0) {
		return "";
	}
	else {
		std::string returnValues;
		for (int argumentIndex = 1; argumentIndex <= lua_gettop(m_mainState); argumentIndex++) {
			
			if (lua_isstring(m_mainState, argumentIndex)) {
				returnValues += lua_tostring(m_mainState, argumentIndex);
			}
			else if (lua_istable(m_mainState, argumentIndex)) {
				returnValues += pickleTable(m_mainState, argumentIndex);
			}
			else {
				returnValues += getType(m_mainState, argumentIndex);
			}

			if (argumentIndex != lua_gettop(m_mainState))
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

	// Math
	lua_createtable(mainState, 0, 2);

	lua_pushstring(mainState, "randomInt");
	lua_pushcfunction(mainState, l_randomInt);
	lua_settable(mainState, -3);

	lua_pushstring(mainState, "randomReal");
	lua_pushcfunction(mainState, l_randomReal);
	lua_settable(mainState, -3);

	lua_setglobal(mainState, "Math");

	// File
	lua_createtable(mainState, 0, 1);

	lua_pushstring(mainState, "readStringFromFile");
	lua_pushcfunction(mainState, l_readStringFromFile);
	lua_settable(mainState, -3);

	lua_setglobal(mainState, "File");

	// Debug
	lua_createtable(mainState, 0, 3);

	lua_pushstring(mainState, "log");
	lua_pushcfunction(mainState, l_debugLog);
	lua_settable(mainState, -3);

	lua_pushstring(mainState, "logWarning");
	lua_pushcfunction(mainState, l_debugLogWarning);
	lua_settable(mainState, -3);

	lua_pushstring(mainState, "logError");
	lua_pushcfunction(mainState, l_debugLogError);
	lua_settable(mainState, -3);

	lua_setglobal(mainState, "Debug");
	

	// Vectors
	NEW_FUNCTION("Vec", l_vecConstruct);
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

int LuaInterpreter::l_randomInt(lua_State* functionState) {

	if (!assertArguments(functionState, "Math.randomInt", { 
		{LUA_TNUMBER }, 
		{LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	int arguments = lua_gettop(functionState);
	if (arguments == 1) {
		int max = lua_tointeger(functionState, 1);
		lua_pushinteger(functionState, (int)(((double)rand() / RAND_MAX)*max) + 1);
		
		return 1;
	}
	else if (arguments == 2) {
		int min = lua_tonumber(functionState, 1);
		int max = lua_tonumber(functionState, 2);
		
		lua_pushinteger(functionState, (int)(((double)rand() / RAND_MAX) * max-min) +min);
		return 1;
	}
	return 0;
}

int LuaInterpreter::l_randomReal(lua_State* functionState) {

	if (!assertArguments(functionState, "Math.randomReal", {
		{LUA_TNUMBER},
		{LUA_TNUMBER, LUA_TNUMBER}
	})) return 0;

	int arguments = lua_gettop(functionState);
	if (arguments == 1) {

		double max = lua_tonumber(functionState, 1);
		lua_pushinteger(functionState, ((double)rand() / RAND_MAX) * max);
		return 1;
	}
	else if (arguments == 2) {

		double min = lua_tonumber(functionState, 1);
		double max = lua_tonumber(functionState, 2);
		lua_pushnumber(functionState, (int)(((double)rand() / RAND_MAX) * max-min) + min);
		return 1;
	}
	return 0;
}

int LuaInterpreter::l_readStringFromFile(lua_State* functionState) {

	if (!assertArguments(functionState, "File.readStringFromFile", {
		{LUA_TSTRING}
	})) return 0;

	if (lua_gettop(functionState) == 1) {

		std::string fileText = cocos2d::FileUtils::getInstance()->getStringFromFile(lua_tostring(functionState, 1));

		// checking if file io succeeded
		if (fileText.empty()) {
			Debugger::logWarning(std::string("File at: ") + lua_tostring(functionState, 1) + " found empty, check if it exists", DEBUG_IO);
		}

		lua_pushstring(functionState, fileText.c_str());
		return 1;
	}
	return 0;
}

int LuaInterpreter::l_debugLog(lua_State* functionState) {

	if (!assertArguments(functionState, "Debug.log", {
		{LUA_TSTRING}
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		
		std::string message = lua_tostring(functionState, 1);
		Debugger::log(message, DEBUG_LUA);
	}
	return 0;
}

int LuaInterpreter::l_debugLogWarning(lua_State* functionState) {

	if (!assertArguments(functionState, "Debug.logWarning", {
		{LUA_TSTRING}
	})) return 0;

	if (lua_gettop(functionState) == 1) {
		
		std::string message = lua_tostring(functionState, 1);
		Debugger::logWarning(message, DEBUG_LUA);
		
	}
	return 0;
}

int LuaInterpreter::l_debugLogError(lua_State* functionState) {

	if (!assertArguments(functionState, "Debug.logError", {
		{ LUA_TSTRING }
	})) return 0;

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

bool LuaInterpreter::assertArguments(lua_State* state,std::string functionName, std::list<std::list<int>> overloads, bool memberMode) {
	
	for (auto currentOverload : overloads) {
		bool overloadValid = true;
		int argumentIndex = memberMode? 2:1;

		if (currentOverload.size() != lua_gettop(state) - memberMode? 1:0) continue; // if the number of paramaters do not match, it is definitely not a valid overload, continue

		for (auto currentParameter : currentOverload) {
			if (currentParameter == LUA_TBOOLEAN) {
				if (!lua_isboolean(state, argumentIndex)) { // if parameter does not match, try next overload
					overloadValid = false;
					break;
				}
			}
			else if (currentParameter == LUA_TNUMBER) {
				if (!lua_isnumber(state, argumentIndex)) {
					overloadValid = false;
					break;
				}
			}
			else if (currentParameter == LUA_TSTRING) {
				if (!lua_isstring(state, argumentIndex)) {
					overloadValid = false;
					break;
				}
			}
			else if (currentParameter == LUA_TTABLE) {
				if (!lua_istable(state, argumentIndex)) {
					overloadValid = false;
					break;
				}
			}

			argumentIndex++;
		}
		if (overloadValid) // if all parameters match, give it an all go!
			return true;
	}

	// no overloads match, print a helpful message
	std::string errorMessage = "No instance of " + functionName + " matches the argument list: (";

	// printing used overload
	for (int i = memberMode?2:1; i <= lua_gettop(state); i++) {
		errorMessage.append(getType(state, i));
		if (i != lua_gettop(state))
			errorMessage.append(", ");
	}

	// printing valid overloads
	errorMessage.append(")\n\toverloads of " + functionName + ":");
	for (auto overload : overloads) {
		errorMessage.append( "\n\t"+functionName + "(");
		int parameterIndex = 0;
		for (auto parameter : overload) {

			errorMessage.append(stringOfLuaType(parameter));
			if (parameterIndex != overload.size() - 1) // if we are not on our last parameter
				errorMessage.append(", ");

			parameterIndex++;
		}
		errorMessage.append(")");
	}

	luaError(state, errorMessage);
	return false;
}

std::string LuaInterpreter::getType(lua_State* state, int index) {
	return stringOfLuaType(lua_type(state, index));
	
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
	else
		return "none";
}

void LuaInterpreter::luaError(lua_State* state, std::string message) {
	std::string errorMessage;
	errorMessage.append("Lua error: " + message);

	lua_getglobal(state, "debug");
	lua_getfield(state, -1, "traceback");
	lua_remove(state, -2);

	lua_pcall(state, 0, 1, 0);
	std::string traceback = lua_tostring(state, -1);
	lua_pop(state, 1);
	if(traceback != "stack traceback:")
		errorMessage.append("\n\t"+traceback);

	Debugger::logError(errorMessage, DEBUG_LUA);
}


void LuaInterpreter::pushVector(lua_State* state, Vec2d vector){
	lua_createtable(state, 0, 2);
	
	lua_pushstring(state, "x");
	lua_pushnumber(state, vector.x);
	lua_settable(state, -3);

	lua_pushstring(state, "y");
	lua_pushnumber(state, vector.y);
	lua_settable(state, -3);

	// Vector meta-methods
	lua_createtable(state, 0, 10);
	
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


int LuaInterpreter::l_vecConstruct(lua_State* functionState){
	if(!assertArguments(functionState, "Vec", {
			{  },
			{ LUA_TNUMBER, LUA_TNUMBER }
			})) return 0;
	
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
