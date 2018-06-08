#include "PackageManager.h"
#include <yaml-cpp/yaml.h>
#include "ResourceMacros.h"
#include <cocos2d.h>
#include <sstream>

PackageManager* PackageManager::m_packageManagerInstance = nullptr;

PackageManager::PackageManager(){
	std::string source = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGE_INDEX);
	YAML::Node root = YAML::Load(source);

	for (auto child : root) {
		std::string packageFolder = child.as<std::string>();
		m_packages.push_back(new Package(packageFolder));
	}
}

std::list<std::string> PackageManager::getXmlScriptIndexes() {
	std::list<std::string> re;

  re.push_back("script/xml/xml_index.yml");

	return re;
}

std::list<std::string> PackageManager::getLuaScriptIndexes() {
	std::list<std::string> re;

  re.push_back("script/lua/lua_index.yml");

	return re;
}

std::string PackageManager::getSprite(std::string spriteName, SpriteType spriteType) {
  std::string prospectiveSpritePath = "res/sprites/";
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

	// We got here... OH NO!!! NO SPRITE!
	return "";
}

std::string PackageManager::getMusic(std::string songName) {
	
  std::string prospectiveFilePath = "res/music/" + songName;

  bool fileExists = cocos2d::FileUtils::getInstance()->isFileExist(prospectiveFilePath);
  
  if (fileExists) {
    return prospectiveFilePath;
  }


	// Oh no's
	return "";
}

std::string PackageManager::getFile(std::string filePath) {

  std::string prospectiveFile = filePath;
  if (cocos2d::FileUtils::getInstance()->isFileExist(prospectiveFile)) {
    return prospectiveFile;
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
		packageInfo.author = package->getAuthor();
		packageInfo.folder = package->getFolder();
		packageInfo.name = package->getName();
		packageInfo.version = package->getVersion();
		
		if (cocos2d::FileUtils::getInstance()->isFileExist(package->getFolderFull() + "/icon.png"))
			packageInfo.icon = package->getFolderFull() + "/icon.png";

		re.push_back(packageInfo);
	}

	return re;
}

void PackageManager::downloadPackageBase() {
	m_downloading = true;

	Package* basePackage = new Package("Base");
	basePackage->setUrl("https://github.com/Hazelfire/ProjectXBase.git");
	m_packages.push_back(basePackage);

	basePackage->sync();

	std::string source = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGE_INDEX);
	YAML::Node root = YAML::Load(source);
	root.push_back("Base");

	std::stringstream ss;
	ss << root;

	cocos2d::FileUtils::getInstance()->writeStringToFile(ss.str(), PACKAGE_INDEX);
}

std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress> PackageManager::getBasePackageProgress() {
	for (auto package : m_packages) {
		if (package->getFolder() == "Base") {
			return package->getSyncProgress();
		}
	}
	return std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress>();
}

/*bool PackageManager::needsUpdate() {
	for (auto package : m_packages) {
		
	}
}*/

bool PackageManager::isInstallingBase() {
	auto progress = getBasePackageProgress();
	if (progress.first.totalObjects == 0) return true;
	if (progress.first.receivedObjects != progress.first.totalObjects) return true;
	if (progress.second.totalSteps != progress.second.completedSteps) return true;

	return false;
}

void PackageManager::lockDownloadBase() {
	m_downloading = true;
}
