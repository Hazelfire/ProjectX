#include "ConfigParser.h"
#include <yaml-cpp/yaml.h>

ConfigValues ConfigParser::parse(std::string script) {
	YAML::Node root = YAML::Load(script.c_str());
	for (YAML::const_iterator currentSection = root.begin(); currentSection != root.end(); currentSection++) {
		std::string sectionName =  currentSection->first.as<std::string>();
	}
	return ConfigValues();
}
