#include "Music.h"
#include <cstring>
#include "XMLCommon.h"
#include "rapidxml.hpp"

using std::strcpy;
using std::strcmp;
using namespace rapidxml;
struct MusicParser::Impl {
	static Requirement parseRequirement(xml_node<> * node);
};

MusicParser::Impl MusicParser::impl;

MusicData MusicParser::parse(string source) {

	MusicData re;

	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());

	xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);

	for (xml_node<> *requirementNode = doc.first_node();
	requirementNode;requirementNode = requirementNode->next_sibling()) {
		
		Requirement currentRequirement = impl.parseRequirement(requirementNode);
		re.requirements.push_back(currentRequirement);
	}
	delete[] newText;
	return re;

}

Requirement MusicParser::Impl::parseRequirement(xml_node<> * node) {
	Requirement re;
	if (strcmp(node->name(), "region") == 0) {
		re.type = "region";
		for (xml_attribute<> *requirementAttribute = node->first_attribute();
		requirementAttribute; requirementAttribute = requirementAttribute->next_attribute()) {
			if (strcmp(requirementAttribute->name(), "name")==0) {
				re.region = requirementAttribute->value();
			}
		}
	}

	for (xml_node<> *childNode = node->first_node();
	childNode;childNode = childNode->next_sibling()) {
		if (strcmp(childNode->name(), "song") == 0) {
			string song = childNode->first_attribute()->value();
			re.songs.push_back(song);
		}
		else {
			re.requirements.push_back(impl.parseRequirement(childNode));
		}
	}
	return re;
}
