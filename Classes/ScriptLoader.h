#pragma once

#include <unordered_map>
#include <list>
#define XML_SCRIPT_SIZE 10

class ScriptLoader {
public:

	typedef std::unordered_map<std::string, std::string> SourceMap;
	typedef std::list<std::string> SourceList;
	enum XmlScriptType{
		XML_CREATURE,
		XML_INTERACTIONS,
		XML_ITEMS,
		XML_MAP,
		XML_MUSIC,
		XML_PLAYERS,
		XML_SPRITES_TILES,
		XML_SPRITES_CREATURES,
		XML_SPRITES_PLAYERS,
		XML_SPRITES_ITEMS,
		XML_SCHEMATICS
	};
	static std::string loadXmlScript(XmlScriptType);

	enum LuaScriptType {
		LUA_INTERACTIONS,
		LUA_STARTUP,
		LUA_CREATURES,
		LUA_ITEMS
	};

	static std::string cachedXMLScripts[XML_SCRIPT_SIZE];

	// finds errors in the xml document, returns valid source files
	static std::list<std::string> findErrorsXML(std::list<std::string> sources);

	static SourceList loadLuaScripts(LuaScriptType);
private:
	static std::string compileXmlScripts(std::list<std::string> sourceFiles);
	static std::string compileLuaScripts(SourceList sourceFiles);
	static std::string runPreprocessor(std::string source);
};
