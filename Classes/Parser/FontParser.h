#include <unordered_map>
#include "math/CCGeometry.h"

struct FontInformation {
	std::unordered_map<char, cocos2d::Rect> characterLocations;
	std::string pictureFile;
	bool antialias;
	int spaceDistance;
};

class FontParser {
public:
	static FontInformation parse(std::string fileName);
};