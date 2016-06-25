#include "Save.h"
#include "Debug.h"
#include <iostream>
#include "base/CCConsole.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

#include <boost/filesystem.hpp>

#endif
#include <fstream>

std::string constructMessage(std::string type, std::string message, DebugMessageType debugType) {
	std::string re;
	re.append("[ " + type + " | ");
	if(debugType == DEBUG_CREATURES)
		re.append("creatures");
	
	else if (debugType == DEBUG_GENERIC) 
		re.append("generic");
	
	else if (debugType == DEBUG_ITEMS) 
		re.append("items");
	
	else if (debugType == DEBUG_MAPGEN) 
		re.append("map generation");
	
	else if (debugType == DEBUG_IO) 
		re.append("io");
	
	else if (debugType == DEBUG_FONTS) 
		re.append("fonts");
	
	else if (debugType == DEBUG_YAML) 
		re.append("yaml");
	
	else if (debugType == DEBUG_PLAYER_SELECTION) 
		re.append("player selection");

	else if (debugType == DEBUG_XML) 
		re.append("xml");

	else if (debugType == DEBUG_MAP_SELECTION)
		re.append("map selection");

	else if (debugType == DEBUG_LUA)
		re.append("lua");

	else if (debugType == DEBUG_INVENTORY)
		re.append("inventory");


	re.append(" ] " + message);
	return re;
}

void appendToFile(std::string file, std::string message) {

	std::fstream logStream(file, std::fstream::app);
	if (logStream.is_open()) {
		logStream << message << "\n";
	}
}

void clearFile(std::string file) {
	std::ofstream fileStream(file);
	fileStream.close();
}

void logToFiles(std::string message) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
	using namespace boost::filesystem;

	path logPath("log");
	create_directory(logPath);

	static bool firstTime = true;
	if (firstTime) {
		clearFile("log/current.log");
		firstTime = false;
	}
	appendToFile("log/current.log", message);
#endif
}

void Debugger::log(std::string message, DebugMessageType type) {
	std::string fullMessage = constructMessage("info", message, type);
	CCLOG(fullMessage.c_str(), "");
	logToFiles(fullMessage);
}

void Debugger::logWarning(std::string message, DebugMessageType type) {
	std::string fullMessage = constructMessage("warning", message, type);
	CCLOG(fullMessage.c_str(), "");
	logToFiles(fullMessage);
}

void Debugger::logError(std::string message, DebugMessageType type) {
	std::string fullMessage = constructMessage("error", message, type);
	CCLOG(fullMessage.c_str(), "");

	logToFiles(fullMessage);
}

