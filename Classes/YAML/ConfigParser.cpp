#include "ConfigParser.h"
#include <yaml-cpp/yaml.h>

ConfigValues ConfigParser::parse(std::string script) {
	YAML::Node root = YAML::Load(script.c_str());
	ConfigValues re;

	if(root["chunkWidth"])
		re.chunkWidth = root["chunkWidth"].as<int>();

	if(root["chunkHeight"])
		re.chunkHeight = root["chunkHeight"].as<int>();	

	if(root["tileWidth"])
		re.tileWidth = root["tileWidth"].as<int>();

	if(root["tileHeight"])
		re.tileHeight = root["tileHeight"].as<int>();

	return re;
}
