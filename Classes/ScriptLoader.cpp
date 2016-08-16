#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include <sstream>
#include <cocos2d.h>
#include "Debug.h"
#include "Parser/ErrorParser.h"
#include "Parser/StringOperations.h"
#include "YAML/ScriptIndex.h"

#define FOLDER_FROM_PATH(path) (path).substr(0,(path).find_last_of("/") +1)

#define FILENAME_FROM_PATH(path) (path).substr((path).find_last_of("/") +1, (path).find_last_of(".") - (path).find_last_of("/") - 1)

std::string ScriptLoader::cachedXMLScripts[XML_SCRIPT_SIZE];

std::string ScriptLoader::loadXmlScript(ScriptLoader::XmlScriptType scriptType) {
	static ScriptIndex xmlScriptIndex = ScriptIndex(XML_INDEX_FILE);

	if (!cachedXMLScripts[scriptType].empty()) {
		return cachedXMLScripts[scriptType];
	}

	SourceList scripts;

	switch (scriptType){
	case XML_CREATURE:
		scripts = xmlScriptIndex.getFiles("creatures");
		break;
	case XML_INTERACTIONS:
		scripts = xmlScriptIndex.getFiles("interactions");
		break;
	case XML_ITEMS:
		scripts = xmlScriptIndex.getFiles("items");
		break;
	case XML_MAP:
		scripts = xmlScriptIndex.getFiles("map");
		break;
	case XML_MUSIC:
		scripts = xmlScriptIndex.getFiles("music");
		break;
	case XML_PLAYERS:
		scripts = xmlScriptIndex.getFiles("players");
		break;
	case XML_SPRITES_ITEMS:
		scripts = xmlScriptIndex.getFiles("sprites/items");
		break;
	case XML_SPRITES_CREATURES:
		scripts = xmlScriptIndex.getFiles("sprites/creatures");
		break;
	case XML_SPRITES_TILES:
		scripts = xmlScriptIndex.getFiles("sprites/tiles");
		break;
	case XML_SPRITES_PLAYERS:
		scripts = xmlScriptIndex.getFiles("sprites/players");
		break;
	case XML_SCHEMATICS:
		scripts = xmlScriptIndex.getFiles("schematics");
		break;
	default:
		// If the script is not of any of these types, the user is being an idiot, it will return empty string
		return std::string();
		break;
	}

	SourceList parsedScripts;

	for (auto script : scripts)
		parsedScripts.push_back(XML_FOLDER + script);

	cachedXMLScripts[scriptType] = compileXmlScripts(parsedScripts);
	

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
	
	static ScriptIndex luaScriptIndex = ScriptIndex(LUA_INDEX_FILE);

	SourceList scripts;

	switch (scriptType) {
	case LUA_INTERACTIONS:
		scripts = luaScriptIndex.getFiles("interactions");
		break;
	case LUA_STARTUP:
		scripts = luaScriptIndex.getFiles("startup");
		break;
	case LUA_CREATURES:
		scripts = luaScriptIndex.getFiles("creatures");
	case LUA_ITEMS:
		scripts = luaScriptIndex.getFiles("items");
	default:
		return SourceList();
		break;
	}
	SourceList re;

	// Append the lua folder onto each entry
	for (auto script : scripts)
		re.push_back(LUA_FOLDER + script);

	return re;
}
