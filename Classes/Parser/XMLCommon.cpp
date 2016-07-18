#include "XMLCommon.h"
#include "base/CCDirector.h"
#include <cocos2d.h>
#include "Debug.h"
#include "StringOperations.h"

using namespace rapidxml;
void safeParse(rapidxml::xml_document<>& doc, const std::string& fileName){

	std::string source = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);
	if (source.empty()) {
		Debugger::logWarning("Found file " + fileName + " empty, make sure it exists", DEBUG_XML);
		
	}

	char* cStr = new char[source.length() + 1];
	strcpy(cStr, source.c_str());

	try {
		doc.parse<0>(cStr);
	}
	catch (rapidxml::parse_error error) {
		long int offset = error.where<char>() - cStr;

		std::string source = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);

		int colomnCount = 0;
		int lineCount = 0;
		int characterIndex = 0;

		while (characterIndex < offset) {
			if (source[characterIndex] == '\n') {
				lineCount++;
				colomnCount = 0;
			}
			else {
				colomnCount++;
			}
			characterIndex++;
		}
		// finally have to colomn and line of the error

		Debugger::logWarning("XML Syntax error for file " + fileName + ". Error reported was " + error.what() + ". Line number: " + StringOps::to_string(lineCount) + ". Colomn Number: " + StringOps::to_string(colomnCount), DEBUG_XML);
	}
}
