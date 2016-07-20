#include "Terrain.h"
#include <string>
#include "XMLCommon.h"
#include "rapidxml.hpp"
#include "Debug.h"
#include <cstring>

using std::strcpy;
using std::strcmp;

struct TerrainParser::Impl {
	static TerrainLayer parseLayer(rapidxml::xml_node<> * layer);
	static RuleTile parseRule(rapidxml::xml_node<> * rule);
	static MapPreset parseMapPreset(rapidxml::xml_node<> * map);
};

using namespace std;
using namespace rapidxml;
TerrainData TerrainParser::parse(string source) {
	TerrainData re;

	char* cStr = new char[source.size() + 1]; // +1 is for null terminator
	strcpy(cStr, source.c_str());
	//Parsing the document using rapidXML
	xml_document<> doc;
	doc.parse<0>(cStr);


	int childCount = 0;
	for (xml_node<> *docChildNode = doc.first_node();
	docChildNode;docChildNode = docChildNode->next_sibling()) {
		if (strcmp(docChildNode->name(), "map") == 0) {
			childCount++;
		}
	}
	// Create an array of size required
	// deletion is never done because it is needed throughout the entire programs life
	re.mapsSize = childCount;
	if (re.mapsSize != 0)
		re.maps = new MapPreset[childCount];

	//Looking through terrain properties
	int childIndex = 0;
	for (xml_node<> *docChildNode = doc.first_node();
	docChildNode;docChildNode = docChildNode->next_sibling()) {
		if (strcmp(docChildNode->name(), "map") == 0) {
			re.maps[childIndex] = impl.parseMapPreset(docChildNode);
			childIndex++;
		}
		else {
			Debugger::logWarning(std::string("Found unrecognised tag in map XML: ") + docChildNode->name(), DEBUG_MAPGEN);
		}
	}
	delete[] cStr;
	return re;
}

MapPreset TerrainParser::Impl::parseMapPreset(rapidxml::xml_node<> * map) {
	MapPreset re;
	for (xml_attribute<> *mapAttr = map->first_attribute();
	mapAttr; mapAttr = mapAttr->next_attribute()) { //Add new properties here
		if (strcmp(mapAttr->name(), "width") == 0) {
			re.width = atoi(mapAttr->value());
		}
		else if (strcmp(mapAttr->name(), "height") == 0) {
			re.height = atoi(mapAttr->value());
		}
		else if (strcmp(mapAttr->name(), "name") == 0) {
			re.name = mapAttr->value();
		}
		else {
			Debugger::logWarning(std::string("Found unrecognised attribute to map tag in map XML: ") + mapAttr->name(), DEBUG_MAPGEN);
		}
	}

	int layerCount = 0;
	for (xml_node<> *layerNode = map->first_node();
	layerNode;layerNode = layerNode->next_sibling()) {
		if (strcmp(layerNode->name(), "layer") == 0) {
			layerCount++;
		}
	}
	re.layersSize = layerCount;
	if (re.layersSize != 0)
		re.layers = new TerrainLayer[re.layersSize];


	//looking at terrain children
	int layerIndex = 0;
	for (xml_node<> *layerNode = map->first_node();
	layerNode;layerNode = layerNode->next_sibling()) {
		if (strcmp(layerNode->name(), "layer") == 0) {
			re.layers[layerIndex] = parseLayer(layerNode);
			layerIndex++;
		}
		else {
			Debugger::logWarning(std::string("Found unrecognised child to map tag in map XML: ") + layerNode->name(), DEBUG_MAPGEN);
		}
	}
	return re;
}

TerrainLayer TerrainParser::Impl::parseLayer(rapidxml::xml_node<>* layer)
{
	TerrainLayer re;
	
	for (xml_attribute<> *layerAttr = layer->first_attribute();
	layerAttr; layerAttr = layerAttr->next_attribute()) { //Add new properties here
		if (strcmp(layerAttr->name(), "name") == 0) {
			re.name = layerAttr->value();
		}
		else {
			Debugger::logWarning(std::string("Found unrecognised attribute to layer tag in map XML: ") + layerAttr->name(), DEBUG_MAPGEN);
		}
	}

	int ruleCount = 0;
	for (xml_node<> *ruleNode = layer->first_node();
	ruleNode;ruleNode = ruleNode->next_sibling()) {
		ruleCount++;
	}
	re.rulesSize = ruleCount;
	if (re.rulesSize != 0)
		re.rules = new RuleTile[re.rulesSize];


	int ruleIndex = 0;
	for (xml_node<> *ruleNode = layer->first_node();
	ruleNode;ruleNode = ruleNode->next_sibling()) {
		re.rules[ruleIndex] = parseRule(ruleNode);
		ruleIndex++;
	}

	return re;
}

RuleTile TerrainParser::Impl::parseRule(rapidxml::xml_node<>* rule)
{
	RuleTile re;
	re.region = "";
	re.name = "";
	for (xml_attribute<> *ruleAttr = rule->first_attribute();
	ruleAttr; ruleAttr = ruleAttr->next_attribute()) {
		if (strcmp(rule->name(), "rule") == 0) {//Add new properties here
			re.ruleTileType = RULE_TYPE;
			if (strcmp(ruleAttr->name(), "type") == 0) {
				re.ruleType = ruleAttr->value();
			}
			else if (strcmp(ruleAttr->name(), "scale") == 0) {
				re.scale = atof(ruleAttr->value());
			}
			else if (strcmp(ruleAttr->name(), "inside") == 0) {
				re.inside = ruleAttr->value();
			}
			else if (strcmp(ruleAttr->name(), "suppBase") == 0) {
				re.supressBase = atof(ruleAttr->value());
			}
			else if (strcmp(ruleAttr->name(), "scaleBase") == 0) {
				re.scaleBase = atof(ruleAttr->value());
			}
			else if (strcmp(ruleAttr->name(), "iter") == 0) {
				re.iterations = atoi(ruleAttr->value());
			}
		}
		else if (strcmp(rule->name(), "tile") == 0) {
			re.ruleTileType = TILE_TYPE;
			if (strcmp(ruleAttr->name(), "name") == 0) {
				re.name = ruleAttr->value();
			}
		}
		else if (strcmp(rule->name(), "schem") == 0) {
			re.name = rule->first_attribute("name")->value();
			re.ruleTileType = SCHEM_TYPE;
		}
		else if (strcmp(rule->name(), "blank") == 0) {
			re.ruleTileType = BLANK_TYPE;
		}
		else if (strcmp(rule->name(), "area") == 0) {
			re.ruleTileType = AREA_TYPE;
		}
		if (strcmp(ruleAttr->name(), "region") == 0) {
			re.region = ruleAttr->value();
		}
		else if (strcmp(ruleAttr->name(), "height") == 0) {
			re.height = atof(ruleAttr->value());
		}
	}
	
	int ruleCount = 0;
	for (xml_node<> *ruleChild = rule->first_node();
	ruleChild;ruleChild = ruleChild->next_sibling()) {
		ruleCount++;
	}
	re.ruleTilesSize = ruleCount;
	if (re.ruleTilesSize != 0)
		re.ruleTiles = new RuleTile[re.ruleTilesSize];

	int ruleIndex = 0;
	for (xml_node<> *ruleChild = rule->first_node();
	ruleChild;ruleChild = ruleChild->next_sibling()) {
			re.ruleTiles[ruleIndex] = parseRule(ruleChild);
			ruleIndex++;
	}

	return re;
}
