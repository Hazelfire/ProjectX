#include "ScriptIndex.h"
#include <CCFileUtils.h>
#include <yaml-cpp/yaml.h>
#include "../Parser/StringOperations.h"
#include  <sstream>
#include "Debug.h"
#include "CCPlatformMacros.h"
#include "ResourceMacros.h"
#define IS_MOBILE (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#if !IS_MOBILE
#include <boost/filesystem.hpp>
#endif

bool ScriptParser::m_directoriesChecked = false;

ScriptParser::XmlScriptIndex ScriptParser::parseXml(std::string fileName) {

	static bool indexLoaded = false;
	std::string source;
	if (!indexLoaded)
		source = createScriptIndex(fileName);
	else
		source = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);
	indexLoaded = true;

	// Loads XML script into YAML
	YAML::Node root = YAML::Load(source.c_str());
	
	XmlScriptIndex re;

	// Running through all the sections
	for (YAML::const_iterator currentSectionNode = root.begin(); currentSectionNode != root.end(); currentSectionNode++) {
		std::string sectionName = currentSectionNode->first.as<std::string>();
		YAML::Node scriptsNode = currentSectionNode->second;

		if (scriptsNode.IsSequence()) {

			for (YAML::const_iterator currentScriptNode = scriptsNode.begin(); currentScriptNode != scriptsNode.end(); currentScriptNode++) {
				if (sectionName == "creatures") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "creatures/" + currentScriptNode->as<std::string>());
					re.creatures.push_back(fullPath);
				}
				else if (sectionName == "interactions") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "interactions/" + currentScriptNode->as<std::string>());
					re.interactions.push_back(fullPath);
				}
				else if (sectionName == "items") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "items/" + currentScriptNode->as<std::string>());
					re.items.push_back(fullPath);
				}
				else if (sectionName == "map") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "map/" + currentScriptNode->as<std::string>());
					re.map.push_back(fullPath);
				}
				else if (sectionName == "music") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "music/" + currentScriptNode->as<std::string>());
					re.music.push_back(fullPath);
				}
				else if (sectionName == "players") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "players/" + currentScriptNode->as<std::string>());
					re.players.push_back(fullPath);
				}
				else if (sectionName == "sprites" ) {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "sprites/" + currentScriptNode->as<std::string>());
					re.sprites.push_back(fullPath);
				}
				else if (sectionName == "schematics") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(XML_FOLDER "schematics/" + currentScriptNode->as<std::string>());
					re.schematics.push_back(fullPath);
				}
			}
		}
	}
	return re;
}

ScriptParser::LuaScriptIndex ScriptParser::parseLua(std::string fileName) {

	static bool indexLoaded = false;

	std::string source;
	if (!indexLoaded)
		source =  createScriptIndex(fileName);
	else
		source = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);
	indexLoaded = true;

	
	YAML::Node root = YAML::Load(source.c_str());

	LuaScriptIndex re;

	// Running through the index
	for (YAML::const_iterator currentSectionNode = root.begin(); currentSectionNode != root.end(); currentSectionNode++) {
		std::string sectionName = currentSectionNode->first.as<std::string>();
		YAML::Node scriptsNode = currentSectionNode->second;

		if (scriptsNode.IsSequence()) {

			for (YAML::const_iterator currentScriptNode = scriptsNode.begin(); currentScriptNode != scriptsNode.end(); currentScriptNode++) {
				if (sectionName == "creatures") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LUA_FOLDER "creatures/" + currentScriptNode->as<std::string>());
					re.creatures.push_back(fullPath);
				}
				else if (sectionName == "interactions") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LUA_FOLDER "interactions/" + currentScriptNode->as<std::string>());
					re.interactions.push_back(fullPath);
				}
				else if (sectionName == "startup") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LUA_FOLDER "startup/" + currentScriptNode->as<std::string>());
					re.startup.push_back(fullPath);
				}
				else if (sectionName == "items") {
					std::string fullpath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LUA_FOLDER "items/" + currentScriptNode->as<std::string>());
					re.items.push_back(fullpath);
				}
				else if (sectionName == "creatureActions") {
					std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LUA_FOLDER "creatureActions/" + currentScriptNode->as<std::string>());
					re.creatureInteractions.push_back(fullPath);
				}
			}
		}
	}
	return re;
}

std::list<std::string> getAllFilesInFolder(std::string prefix,std::string folder) {
	std::list<std::string> re;
#if !IS_MOBILE
	using namespace boost::filesystem;
	if (exists(folder)) {
		if (is_regular_file(folder)) {
			re.push_back(folder.substr(prefix.size() + 1, folder.size() - prefix.size() - 1));
		}
		else if (is_directory(folder)) {
			for (directory_iterator currentChild(folder); currentChild != directory_iterator(); currentChild++) {
				re.splice(re.end(), getAllFilesInFolder(prefix, currentChild->path().generic_string()));
			}
		}
	}
#endif
	return re;
}


#define FOLDER_FROM_PATH(path) (path).substr(0,(path).find_last_of("/") +1)
#define FINAL_FOLDER_FROM_PATH(path) (path).substr((path).find_last_of("/") +1,(path).size() - (path).find_last_of("/"))

// Creates a script index based on what is already in the files, not done
std::string ScriptParser::createScriptIndex(std::string fileName) {
#if !IS_MOBILE

	YAML::Node document;
	

	using namespace boost::filesystem;
	// Linux does not empty the resources folder into the excecutable's directory
	// but instead inside a Resources folder
	std::string directory = FOLDER_FROM_PATH(cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName));
	if (exists(directory) && is_directory(directory)) {
		for (directory_iterator currentChild(directory); currentChild != directory_iterator(); currentChild++) {
			if (is_directory(currentChild->path())) {
				std::string folderName = currentChild->path().generic_string();
				std::list<std::string> files = getAllFilesInFolder(folderName, folderName);
				document[FINAL_FOLDER_FROM_PATH(folderName)] = files;
			}
		}
	}else{
		Debugger::logError("Failed to traverse script directories", DEBUG_IO);
	}
	std::stringstream ss;
	ss << document;


	cocos2d::FileUtils::getInstance()->writeStringToFile(ss.str(), fileName);
	return ss.str();
#endif
}
