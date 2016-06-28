#include <string>
struct ConfigValues{
	int chunkWidth = 20;
	int chunkHeight = 20;
	int tileHeight = 24;
	int tileWidth = 32;
};

class ConfigParser {
	static ConfigValues parse(std::string script);
};
