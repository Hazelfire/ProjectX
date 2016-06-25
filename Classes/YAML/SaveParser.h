#include "SaveInformation.h"

// Class for saving YAML save Files
class YAMLSave {
public:
	// Reads save Information from a file
	static SaveInformation readSave(std::string fileLocation);

	// Writes save Information into a file
	static void writeSave(SaveInformation, std::string fileLocation);
};