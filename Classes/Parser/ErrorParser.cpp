#include "ErrorParser.h"
#include <cstring>
#include "rapidxml.hpp"

using namespace rapidxml;

XMLErrorParser::XMLError XMLErrorParser::parse(std::string source) {
	
	char* cStr = new char[source.length() +1];
	std::strcpy(cStr, source.c_str());

	// parsing
	xml_document<> doc;
	try {
		doc.parse<0>(cStr);
	}
	catch (parse_error error) {
		long int offset = error.where<char>() - cStr;

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
		XMLError returnError;
		returnError.isError = true;
		returnError.message = error.what();
		returnError.marker.lineNumber = lineCount;
		returnError.marker.colomnNumber = colomnCount;
		
		return returnError;
	}
	return XMLError();
}
