#ifndef __HELLOWORLD_FILE_OPERATION__
#define __HELLOWORLD_FILE_OPERATION__

#include <string>

class FileOperation 
{
public:
	static void saveFile(std::string filePath, std::string data);
	static std::string readFile(std::string filePath);
	static std::string getFilePath();
};

#endif
