#include "Package.h"
#include <yaml-cpp/yaml.h>
#include "ResourceMacros.h"
#include <cocos2d.h>
#include <boost/thread.hpp>

// Returns a package object from the folder given (relative to the package folder)
Package::Package(std::string folder) {
	m_folder = folder;
	if (cocos2d::FileUtils::getInstance()->isDirectoryExist(PACKAGES_FOLDER + folder)) {
		std::string yamlSource = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGES_FOLDER + folder + "/package.yml");
		YAML::Node root = YAML::Load(yamlSource);

		m_name = root["name"].as<std::string>();
		m_author = root["author"].as<std::string>();

		m_version = root["version"].as<std::string>();
		m_gitRepo = new GitRepository(folder);
	}
}

bool Package::sync() {
	if (m_remote.empty()) return false;

	if (!m_gitRepo) {

		m_gitRepo = new GitRepository();
		// no folder, lets clone
		boost::thread thread([this]() {
			
		m_gitRepo->clone(m_remote, PACKAGES_FOLDER + m_folder);
		
		std::string yamlSource = cocos2d::FileUtils::getInstance()->getStringFromFile(PACKAGES_FOLDER + m_folder + "/package.yml");
		YAML::Node root = YAML::Load(yamlSource);

		m_name = root["name"].as<std::string>();
		m_author = root["author"].as<std::string>();

		m_version = root["version"].as<std::string>();
		m_gitRepo = new GitRepository(m_folder);
		});
		thread.join();
	}
}

void Package::setUrl(std::string url) {
	m_remote = url;
}

void Package::lockDownload() {
	m_isDownloading = true;
}

bool Package::isDownloading() {
	return m_isDownloading;
}

std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress> Package::getSyncProgress() {
	if (m_gitRepo)
		return std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress>(m_gitRepo->getDownloadProgress(), m_gitRepo->getCheckoutProgress());
	else
		return std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress>();
}

std::string Package::getXmlScriptIndex() {
	return PACKAGES_FOLDER + m_folder + "/script/xml/xml_index.yml";
}

std::string Package::getLuaScriptIndex() {
	return PACKAGES_FOLDER + m_folder + "/script/lua/lua_index.yml";
}

std::string Package::getFolder() {
	return m_folder;
}

std::string Package::getFolderFull() {
	return PACKAGES_FOLDER + m_folder;
}

std::string Package::getName() {
	return m_name;
}

std::string Package::getAuthor() {
	return m_author;
}

std::string Package::getVersion() {
	return m_version;
}
