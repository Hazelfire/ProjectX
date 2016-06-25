#include "HashDirectory.h"
#include <boost/filesystem.hpp>
#include <CCFileUtils.h>

uint32_t myHash(std::string const& key)
{
	uint32_t results = 12345;
	for (auto current = key.begin(); current != key.end(); ++current) {
		results = 127 * results + static_cast<unsigned char>(*current);
	}
	return results;

}

size_t hashDirectory(std::string directory) {
	using namespace boost::filesystem;
	path directoryPath(directory);
	if(is_directory(directoryPath)){
		size_t hash = 0;
		for (directory_iterator currentChild(directoryPath); currentChild != directory_iterator(); currentChild++) {
			hash = hash ^ hashDirectory(currentChild->path().generic_string());
		}
		return hash;
	}
	else if (is_regular_file(directory)) {
		std::string contents = cocos2d::FileUtils::getInstance()->getStringFromFile(directory);
		return myHash(contents);
	}
}