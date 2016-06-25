// to enable CCLOG()
#define COCOS2D_DEBUG 1

#include "cocos2d.h"
#include "FileOperation.h"
#include <stdio.h>
#include "Debug.h"

using namespace std;

void FileOperation::saveFile(std::string filePath, std::string data)
{
	Debugger::log("Saving into file: " + filePath, DEBUG_IO);
	string path = getFilePath() + filePath;
	if (!cocos2d::FileUtils::getInstance()->writeStringToFile(data, path)) {
		Debugger::logWarning("Failed to write string to file " + filePath, DEBUG_IO);
	}
}

std::string FileOperation::readFile(std::string filePath)
{
	string path = getFilePath() + filePath;
	std::string fileContents = cocos2d::FileUtils::getInstance()->getStringFromFile(path);
	if (fileContents.empty()) {
		Debugger::logWarning("Reading file " + filePath  + " returned an empty string, file may not exist", DEBUG_IO);
	}
	return fileContents;
}

string FileOperation::getFilePath()
{
	string path("");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// In android, every programe has a director under /data/data.
	// The path is /data/data/ + start activity package name.
	// You can save application specific data here.
	path.append("/data/data/projectx.hazelfire.visualstudio.com/");

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	// You can save file in anywhere if you have the permision.

	// I have chosen the save directory next to the application
	path.append("save/");

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)
	path = cocos2d::CCApplication::sharedApplication().getAppDataPath();

#ifdef _TRANZDA_VM_
	// If runs on WoPhone simulator, you should insert "D:/Work7" at the
	// begin. We will fix the bug in no far future.
	path = "D:/Work7" + path;
#endif

#else
	path = cocos2d::FileUtils::getInstance()->getWritablePath();
#endif

	return path;
}
