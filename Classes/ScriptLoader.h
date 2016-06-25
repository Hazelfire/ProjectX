#pragma once
#include "YAML/ScriptIndex.h"
#include <unordered_map>
#define XML_SCRIPT_SIZE 8
#define LUA_SCRIPT_SIZE 2
#define LUA_SEPERATE_SCRIPT_SIZE 1

class ScriptLoader {
public:

	typedef std::unordered_map<std::string, std::string> SourceMap;
	enum XmlScriptType{
		XML_CREATURE,
		XML_INTERACTIONS,
		XML_ITEMS,
		XML_MAP,
		XML_MUSIC,
		XML_PLAYERS,
		XML_SPRITES,
		XML_SCHEMATICS
	};
	static std::string loadXmlScript(XmlScriptType);

	enum LuaScriptType {
		LUA_INTERACTIONS,
		LUA_STARTUP
	};
	enum SeperateLuaScriptType {
		LUA_CREATURES
	};

	static std::string cachedXMLScripts[XML_SCRIPT_SIZE];
	static std::string cachedLuaScripts[LUA_SCRIPT_SIZE];
	static SourceMap cachedLuaSeperateScripts[LUA_SEPERATE_SCRIPT_SIZE];

	// finds errors in the xml document, returns valid source files
	static std::list<std::string> findErrorsXML(std::list<std::string> sources, std::string filePrefix);

	static SourceMap loadSeperateLuaScript(SeperateLuaScriptType);
	static SourceMap openScripts(std::list<std::string>);
	static std::string loadLuaScript(LuaScriptType);
private:
	static ScriptParser::XmlScriptIndex m_XmlScriptIndex;
	static ScriptParser::LuaScriptIndex m_LuaScriptIndex;
	static std::string compileXmlScripts(std::list<std::string> sourceFiles, std::string folderName);
	static std::string compileLuaScripts(std::list<std::string> sourceFiles);
	static std::string runPreprocessor(std::string source, std::string filePrefix);
};
