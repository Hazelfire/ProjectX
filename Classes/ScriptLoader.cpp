#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include <sstream>
#include <cocos2d.h>
#include "Debug.h"
#include "Parser/ErrorParser.h"
#include "Parser/StringOperations.h"
#include "YAML/ScriptIndex.h"
#include "Packages/PackageManager.h"

#define FOLDER_FROM_PATH(path) (path).substr(0,(path).find_last_of("/") +1)

#define FILENAME_FROM_PATH(path) (path).substr((path).find_last_of("/") +1, (path).find_last_of(".") - (path).find_last_of("/") - 1)

std::string ScriptLoader::cachedXMLScripts[XML_SCRIPT_SIZE];

std::string ScriptLoader::loadXmlScript(ScriptLoader::XmlScriptType scriptType) {
	std::list<std::string> xmlScriptIndexPaths = PackageManager::getInstance()->getXmlScriptIndexes();

	if (!cachedXMLScripts[scriptType].empty()) {
		return cachedXMLScripts[scriptType];
	}

	SourceList scripts;
	for (auto xmlIndexPath : xmlScriptIndexPaths) {

		ScriptIndex xmlScriptIndex = ScriptIndex(xmlIndexPath);

		std::string scriptFolder;
		switch (scriptType) {
		case XML_CREATURE:
			scriptFolder = "creatures";
			break;
		case XML_INTERACTIONS:
			scriptFolder = "interactions";
			break;
		case XML_ITEMS:
			scriptFolder = "items";
			break;
		case XML_MAP:
			scriptFolder = "map";
			break;
		case XML_MUSIC:
			scriptFolder = "music";
			break;
		case XML_PLAYERS:
			scriptFolder = "players";
			break;
		case XML_SPRITES_ITEMS:
			scriptFolder = "sprites/items";
			break;
		case XML_SPRITES_CREATURES:
			scriptFolder = "sprites/creatures";
			break;
		case XML_SPRITES_TILES:
			scriptFolder = "sprites/tiles";
			break;
		case XML_SPRITES_PLAYERS:
			scriptFolder = "sprites/players";
			break;
		case XML_SCHEMATICS:
			scriptFolder = "schematics";
			break;
		default:
			// If the script is not of any of these types, the user is being an idiot, it will return empty string
			return std::string();
			break;
		}

		// Append list on list
		scripts.splice(scripts.end(), xmlScriptIndex.getFiles(scriptFolder));
	}


	cachedXMLScripts[scriptType] = compileXmlScripts(scripts);
	

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
		sourceStream << cocos2d::FileUtils::getInstance()->getStringFromFile(*currentSource);
	}

	return runPreprocessor(sourceStream.str());
}

ScriptLoader::SourceList ScriptLoader::loadLuaScripts(ScriptLoader::LuaScriptType scriptType) {
	
	std::list<std::string> luaIndexPaths = PackageManager::getInstance()->getLuaScriptIndexes();

	SourceList scripts;

	for (auto indexPath : luaIndexPaths) {

		ScriptIndex luaScriptIndex(indexPath);
		
		std::string scriptFolder;
		switch (scriptType) {
		case LUA_INTERACTIONS:
			scriptFolder = "interactions";
			break;
		case LUA_STARTUP:
			scriptFolder = "startup";
			break;
		case LUA_CREATURES:
			scriptFolder = "creatures";
		case LUA_ITEMS:
			scriptFolder = "items";
		default:
			return SourceList();
			break;
		}

		scripts.splice(scripts.end(), luaScriptIndex.getFiles(scriptFolder));
	}

	return scripts;
}
