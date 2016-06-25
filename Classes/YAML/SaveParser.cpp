#include "SaveParser.h"
#include <yaml-cpp/yaml.h>
#include "FileOperation.h"
#include <sstream>
#include "Debug.h"
#include "Parser/StringOperations.h"

SaveInformation YAMLSave::readSave(std::string fileLocation) {
	std::string source = FileOperation::readFile(fileLocation);
	SaveInformation re;
	if (source.empty()) return re;

	try {
		YAML::Node root = YAML::Load(source.c_str());
		
		if (root["playerName"])
			re.playerName = root["playerName"].as<std::string>();

		if (root["fullScreen"])
			re.fullScreen = root["fullScreen"].as<bool>();
		else
			re.fullScreen = false; // fullscreen is by default, disabled

		if (root["skipSplashes"])
			re.skipSplashes = root["skipSplashes"].as<bool>();

		if (root["devMode"])
			re.devMode = root["devMode"].as<bool>();
		else
			re.skipSplashes = false;
	}
	catch (YAML::Exception exception) {
		Debugger::logWarning("Error parsing YAML Save file: " + exception.msg + " line number: " + StringOps::to_string(exception.mark.line) + ", colomn number: " + StringOps::to_string(exception.mark.column), DEBUG_YAML);
		return re;
	}

	return re;
}

void YAMLSave::writeSave(SaveInformation saveInfo, std::string fileLocation) {
	YAML::Node root;

	root["playerName"] = saveInfo.playerName;
	root["fullScreen"] = saveInfo.fullScreen;
	
	if(saveInfo.skipSplashes) // only write to file if the user wants to skip splashes
		root["skipSplashes"] = saveInfo.skipSplashes;

	if (saveInfo.devMode)
		root["devMode"] = saveInfo.devMode;

	std::stringstream stream;
	stream << root;
	
	FileOperation::saveFile(fileLocation,stream.str());
}
