#include "SchematicsParser.h"
#include "StringOperations.h"
#include <sstream>
#include "XMLCommon.h"
#include "rapidxml.hpp"
#include <cstring>

using std::strcpy;
using std::strcmp;

using namespace rapidxml;

SchematicMap SchematicsParser::parse(std::string source) {

	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());

	xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);

	SchematicMap re;

	for (xml_node<>* schematic = doc.first_node("schem"); schematic; schematic = schematic->next_sibling("schem")) {
		int width = std::atol(schematic->first_attribute("width")->value());
		int height = std::atol(schematic->first_attribute("height")->value());
		
		Schematic currentSchematic;
		for (int layerIndex = 0; layerIndex < 2; layerIndex++) {
			currentSchematic.data[layerIndex].resize(std::atol(schematic->first_attribute("width")->value()));
			for (int i = 0; i < (std::atol(schematic->first_attribute("width")->value())); i++) {
				currentSchematic.data[layerIndex][i].resize(std::atol(schematic->first_attribute("height")->value()));
			}
		}
		for (xml_node<>* layerNode = schematic->first_node(); layerNode; layerNode = layerNode->next_sibling()) {
			std::stringstream sourceStream(layerNode->value());
			std::string item;
			int count = 0;
			StringTable currentLayerInformation;
			
			if (strcmp(layerNode->name(), "bottom") == 0) {
				while (std::getline(sourceStream, item, ',')) {
					std::string trimmedItemName = StringOps::trim(item);
					currentSchematic.data[0][(int)count % width][(int)count / width] = trimmedItemName;
					count++;
				}
			}
			else if (strcmp(layerNode->name(), "top") == 0) {
				while (std::getline(sourceStream, item, ',')) {
					std::string trimmedItemName = StringOps::trim(item);
					currentSchematic.data[1][(int)count % width][(int)count / width] = trimmedItemName;
					count++;
				}
			}
		}
		currentSchematic.width = width;
		currentSchematic.height = height;
		re[schematic->first_attribute("name")->value()] = currentSchematic;
	}
	delete[] newText;
	return re;
}
