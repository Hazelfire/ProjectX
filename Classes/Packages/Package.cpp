#include "Package.h"
#include <yaml-cpp\yaml.h>
#include "ResourceMacros.h"
#include <cocos2d.h>

// Returns a package object from the folder given (relative to the package folder)
Package::Package(std::string folder) {

	std::string yamlSource = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGES_FOLDER + folder + "/package.yml");
	YAML::Node root = YAML::Load(yamlSource);

	m_name = root["name"].as<std::string>();
	m_author = root["author"].as<std::string>();
	m_folder = folder;

	// TODO Version parsing
}


std::string Package::getXmlScriptIndex() {
	return PACKAGES_FOLDER + m_folder + "/script/xml/xml_index.yml";
}

std::string Package::getLuaScriptIndex() {
	return PACKAGES_FOLDER + m_folder + "/script/lua/lua_index.yml";
}

std::string Package::getFolder() {
	return PACKAGES_FOLDER + m_folder;
}