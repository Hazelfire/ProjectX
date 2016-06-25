#include "FontParser.h"
#include "CCFileUtils.h"
#include <sstream>
#include "StringOperations.h"
#include "../ResourceMacros.h"
#include "Debug.h"

FontInformation FontParser::parse(std::string fileName) {
	FontInformation re;
	std::stringstream sourceStream(cocos2d::FileUtils::getInstance()->getStringFromFile(fileName));
	if (sourceStream.str().length() == 0) { // if file failed to load
		Debugger::logError("Failed to load font at file: " + fileName, DEBUG_FONTS);
	}

	std::string section;
	int sectionIndex = 0;
	while (getline(sourceStream, section, '~')) {
		std::stringstream sectionStream(section);
		std::string line;
		if (sectionIndex == 0) {
			while (getline(sectionStream, line, ';')) {
				if (StringOps::trim(line).empty()) continue;
				std::string characters = StringOps::trim(line.substr(0, line.find_last_of(':')));
				std::string rectangle = line.substr(line.find_last_of(':') + 1, line.length());
				std::stringstream rectangleStream(rectangle);
				std::string rectangleAttribute;
				cocos2d::Rect currentRectangle;
				int attributeIndex=0;
				while (getline(rectangleStream, rectangleAttribute, ' ')) {
					std::string attribute = StringOps::trim(rectangleAttribute);
					if (attribute.empty()) continue;
					if (attributeIndex == 0) {
						currentRectangle.origin.x = atoi(attribute.c_str());
					}
					else if (attributeIndex == 1) {
						currentRectangle.origin.y = atoi(attribute.c_str());
					}
					else if (attributeIndex == 2) {
						currentRectangle.size.width = atoi(attribute.c_str());
					}
					else if (attributeIndex == 3) {
						currentRectangle.size.height = atoi(attribute.c_str());
					}
					attributeIndex++;
				}

				for (std::string::iterator currentCharacter = characters.begin(); currentCharacter != characters.end(); currentCharacter++) {
					re.characterLocations[*currentCharacter] = currentRectangle;
				}
			}
		}

		if (sectionIndex == 1) {
			std::stringstream sectionStream(section);
			std::string line;
			while (getline(sectionStream, line, ';')) {
				std::string trimmedLine = StringOps::trim(line);
				if (trimmedLine.empty()) continue;
				std::string propertyToken = trimmedLine.substr(0, trimmedLine.find_last_of(':'));
				std::string valueToken = trimmedLine.substr(trimmedLine.find_last_of(':') + 1, trimmedLine.length());
				if (propertyToken.compare("picture") ==0) {
					re.pictureFile = FONTS_FOLDER + valueToken;
				}
				else if (propertyToken.compare("antialias") == 0) {
					if (valueToken.compare("true") == 0) {
						re.antialias = true;
					}
					else if (valueToken.compare("false") == 0) {
						re.antialias = false;
					}
				}
				else if (propertyToken.compare("spaceDistance") == 0) {
					re.spaceDistance = atoi(valueToken.c_str());
				}
			}
		}
		sectionIndex++;
	}
	return re;
}