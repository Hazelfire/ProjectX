#include "ScriptIndex.h"
#include <cocos2d.h>
#include <yaml-cpp/yaml.h>
#include "../Parser/StringOperations.h"
#include  <sstream>
#include "Debug.h"
#include "ResourceMacros.h"
#define IS_MOBILE (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#if !IS_MOBILE
#include <boost/filesystem.hpp>
#endif

ScriptIndex::ScriptIndex(std::string fileName) {
	std::string source = createScriptIndex(fileName);

	YAML::Node root = YAML::Load(source);

	for (auto currentSectionNode : root) {
		std::string sectionName = currentSectionNode.first.as<std::string>();
		YAML::Node sectionNode = currentSectionNode.second;

		if (sectionNode.IsSequence()) {

			for (auto currentScriptNode : sectionNode) {
				m_files.push_back(sectionName + "/" + currentScriptNode.as<std::string>());
			}

		}
	}
}

std::list<std::string> ScriptIndex::getFiles(std::string directory) {
	std::list<std::string> re;

	for (auto scriptIndex : m_files) {

		// If the directory is at the start of this script
		if (scriptIndex.find(directory) == 0) {
			
			re.push_back(scriptIndex);
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
std::string ScriptIndex::createScriptIndex(std::string fileName) {
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
#else
	return cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);
#endif
}
