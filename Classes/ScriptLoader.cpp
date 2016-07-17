#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include <sstream>
#include <CCFileUtils.h>
#include "Debug.h"
#include "Parser/ErrorParser.h"
#include "Parser/StringOperations.h"

#define FOLDER_FROM_PATH(path) (path).substr(0,(path).find_last_of("/") +1)

#define FILENAME_FROM_PATH(path) (path).substr((path).find_last_of("/") +1, (path).find_last_of(".") - (path).find_last_of("/") - 1)

#define LOAD_LUA_SCRIPT_INDEX() if (m_LuaScriptIndex.interactions.empty()) { \
m_LuaScriptIndex = ScriptParser::parseLua(LUA_INDEX_FILE); \
	} 

#define LOAD_XML_SCRIPT_INDEX() if(m_XmlScriptIndex.interactions.empty()){ \
	m_XmlScriptIndex = ScriptParser::parseXml(XML_INDEX_FILE); \
}

ScriptParser::XmlScriptIndex ScriptLoader::m_XmlScriptIndex;
ScriptParser::LuaScriptIndex ScriptLoader::m_LuaScriptIndex;
std::string ScriptLoader::cachedXMLScripts[XML_SCRIPT_SIZE];

std::string ScriptLoader::loadXmlScript(ScriptLoader::XmlScriptType scriptType) {
	LOAD_XML_SCRIPT_INDEX();

	if (!cachedXMLScripts[scriptType].empty()) {
		return cachedXMLScripts[scriptType];
	}

	switch (scriptType){
	case XML_CREATURE:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.creatures);
		break;
	case XML_INTERACTIONS:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.interactions);
		break;
	case XML_ITEMS:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.items);
		break;
	case XML_MAP:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.map);
		break;
	case XML_MUSIC:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.music);
		break;
	case XML_PLAYERS:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.players);
		break;
	case XML_SPRITES:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.sprites);
		break;
	case XML_SCHEMATICS:
		cachedXMLScripts[scriptType] = compileXmlScripts(m_XmlScriptIndex.schematics);
		break;
	default:
		// If the script is not of any of these types, the user is being an idiot, it will return empty string
		return std::string();
		break;
	}
	return cachedXMLScripts[scriptType];
}

std::list<std::string> ScriptLoader::findErrorsXML(std::list<std::string> sourceFiles) {
	std::list<std::string> re;
	for (auto currentSourceFile : sourceFiles) {
		std::string currentSourceString = cocos2d::FileUtils::getInstance()->getStringFromFile( currentSourceFile);
		if (currentSourceString.empty()) {
			Debugger::logWarning("Found file " + currentSourceFile + " empty, check if it exists", DEBUG_XML);
			continue;
		}
		XMLErrorParser::XMLError error = XMLErrorParser::parse(currentSourceString);
		if (error.isError) {
			Debugger::logError("Found sytax error in XML file "+currentSourceFile+", message is: " + error.message + ": (" + StringOps::to_string(error.marker.lineNumber) + ", " + StringOps::to_string(error.marker.colomnNumber) + ")", DEBUG_XML);
		}
		else {
			re.push_front(currentSourceFile);
		}
	}
	return re;
}

std::string ScriptLoader::compileXmlScripts(std::list<std::string> sources) {
	std::stringstream sourceStream;

	sources = findErrorsXML(sources);

	// Compiles all the source files into one source
	for (auto currentSource : sources) {
		sourceStream << cocos2d::FileUtils::getInstance()->getStringFromFile(currentSource);
	}

	return runPreprocessor(sourceStream.str());
}

std::string ScriptLoader::runPreprocessor(std::string source) {
	std::stringstream sourceStream(source);
	std::string line;
	std::stringstream parsedSource;
	while (getline(sourceStream, line)) {

		// If this line is for the preprocessor
		if (line.empty()) continue;
		if (line.at(0) == '#') {

			if (line.find("file") == 1) {
				std::string filename = line.substr(6, line.length() - 7);
				std::string folderFromPath = FOLDER_FROM_PATH(filename);
				parsedSource << runPreprocessor(cocos2d::FileUtils::getInstance()->getStringFromFile(filename));
			}
		}
		else {
			parsedSource << line << "\n";
		}
	}
	return parsedSource.str();
}

std::string ScriptLoader::compileLuaScripts(std::list<std::string> sources) {
	std::stringstream sourceStream;

	// Compiles all the source files into one source
	for (std::list<std::string>::iterator currentSource = sources.begin(); currentSource != sources.end(); currentSource++) {
		sourceStream << cocos2d::FileUtils::getInstance()->getStringFromFile(LUA_FOLDER + *currentSource);
	}

	return runPreprocessor(sourceStream.str());
}

ScriptLoader::SourceList ScriptLoader::loadLuaScripts(ScriptLoader::LuaScriptType scriptType) {
	LOAD_LUA_SCRIPT_INDEX();
	switch (scriptType) {
	case LUA_INTERACTIONS:
		return m_LuaScriptIndex.interactions;
		break;
	case LUA_STARTUP:
		return m_LuaScriptIndex.startup;
		break;
	case LUA_CREATURES:
		return m_LuaScriptIndex.creatures;
	case LUA_ITEMS:
		return m_LuaScriptIndex.items;
	case LUA_CREATURE_ACTIONS:
		return m_LuaScriptIndex.creatureInteractions;
	default:
		return SourceList();
		break;
	}
}
