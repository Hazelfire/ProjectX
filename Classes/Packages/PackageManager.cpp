#include "PackageManager.h"
#include <yaml-cpp\yaml.h>
#include "ResourceMacros.h"
#include <cocos2d.h>

PackageManager* PackageManager::m_packageManagerInstance = nullptr;

PackageManager::PackageManager(){
	std::string source = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGE_INDEX);
	YAML::Node root = YAML::Load(source);

	for (auto child : root) {
		std::string packageFolder = child.as<std::string>();
		m_packages.push_back(Package(packageFolder));
	}
}

std::list<std::string> PackageManager::getXmlScriptIndexes() {
	std::list<std::string> re;

	for (auto package : m_packages) {
		re.push_back(package.getXmlScriptIndex());
	}

	return re;
}

std::list<std::string> PackageManager::getLuaScriptIndexes() {
	std::list<std::string> re;

	for (auto package : m_packages) {
		re.push_back(package.getLuaScriptIndex());
	}

	return re;
}

std::string PackageManager::getSprite(std::string spriteName, SpriteType spriteType) {
	for (auto package : m_packages) {
		std::string prospectiveSpritePath = package.getFolderFull() + "/res/sprites/";
		if (spriteType == SPRITE_CREATURE) {
			prospectiveSpritePath += "creatures";
		}
		else if (spriteType == SPRITE_TILE) {
			prospectiveSpritePath += "tiles";
		}
		else if (spriteType == SPRITE_PLAYER) {
			prospectiveSpritePath += "players";
		}
		else if (spriteType == SPRITE_ITEM) {
			prospectiveSpritePath += "items";
		}
		prospectiveSpritePath += "/" + spriteName;

		bool fileExists = cocos2d::FileUtils::getInstance()->isFileExist(prospectiveSpritePath);

		// YAY! This IS the image we were looking for
		if (fileExists)
			return prospectiveSpritePath;
	}

	// We got here... OH NO!!! NO SPRITE!
	return "";
}

std::string PackageManager::getMusic(std::string songName) {
	
	for (auto package : m_packages) {
		std::string prospectiveFilePath = package.getFolderFull() + "/res/music/" + songName;

		bool fileExists = cocos2d::FileUtils::getInstance()->isFileExist(prospectiveFilePath);
		
		if (fileExists) {
			return prospectiveFilePath;
		}
	}


	// Oh no's
	return "";
}

std::string PackageManager::getFile(std::string filePath) {

	for (auto package : m_packages) {
		std::string prospectiveFile = package.getFolderFull() + filePath;
		if (cocos2d::FileUtils::getInstance()->isFileExist(prospectiveFile)) {
			return prospectiveFile;
		}
	}
	return "";
}

// Instance management
PackageManager* PackageManager::getInstance() {
	if (!m_packageManagerInstance)
		m_packageManagerInstance = new PackageManager();

	return m_packageManagerInstance;
}

void PackageManager::deleteInstance() {
	delete m_packageManagerInstance;
}

std::list<PackageManager::PackageInfo> PackageManager::getPackageInfo() {
	std::list<PackageInfo> re;

	for (auto package : m_packages) {
		PackageInfo packageInfo;
		packageInfo.author = package.getAuthor();
		packageInfo.folder = package.getFolder();
		packageInfo.name = package.getName();
		packageInfo.version = package.getVersion();
		
		if (cocos2d::FileUtils::getInstance()->isFileExist(package.getFolderFull() + "/icon.png"))
			packageInfo.icon = package.getFolderFull() + "/icon.png";

		re.push_back(packageInfo);
	}

	return re;
}