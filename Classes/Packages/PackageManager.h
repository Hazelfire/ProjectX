#include <list>
#include <string>
#include "Package.h"
#include "SpriteTypes.h"

class PackageManager {
public:

	std::list<std::string> getXmlScriptIndexes();
	std::list<std::string> getLuaScriptIndexes();

	struct PackageInfo {
		std::string name;
		std::string folder;
		std::string author;
		std::string icon;
		std::string version;
	};

	// Returns the full path of filepath relative to a package
	std::string getFile(std::string filePath);

	// Returns the full filepath of the sprite
	std::string getSprite(std::string spriteName, SpriteType spriteType);

	// Returns the full filepath of the song
	std::string getMusic(std::string songName);

	// Returns metadata in regards to the packages
	std::list<PackageInfo> getPackageInfo();

	PackageManager();

	// Instance management
	static PackageManager* getInstance();
	static void deleteInstance();
private:

	static PackageManager* m_packageManagerInstance;
	
	std::list<Package> m_packages;
};