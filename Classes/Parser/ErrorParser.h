#include <string>

class XMLErrorParser {
public:
	struct XMLError {
		bool isError = false;
		std::string message;
		struct XMLErrorMarker {
			int lineNumber;
			int colomnNumber;
		};
		XMLErrorMarker marker;
	};
	
	static XMLError parse(std::string source);

};