#include "Responces.h"
#include "StringOperations.h"
#include "../ResourceMacros.h"
#include "XMLCommon.h"
#include <sstream>
#include "rapidxml.hpp"
#include <cstring>

using std::strcpy;
using std::strcmp;

struct InteractionsParser::Impl {
	static Interaction parseInteractions(rapidxml::xml_node<>*);
};

InteractionsParser::Impl InteractionsParser::impl;

using namespace rapidxml;
ActionMap InteractionsParser::parse(string source) {
	ActionMap re;
	
	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());
	
	// Parsing
	xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);

	// Movable tags
	xml_node<>* movableTag = doc.first_node("movable");
	if (movableTag) {
		std::stringstream movableTiles(std::string(movableTag->value()));
		string line;
		while (std::getline(movableTiles, line, ';')) {
			line = StringOps::trim(line);
			if (line.empty()) continue;
			re.tiles[line].isMovable = true;
		}
	}


	for (xml_node<> *interactionNode = doc.first_node();
	interactionNode;interactionNode = interactionNode->next_sibling()) {
		std::string interactionType = interactionNode->name();

		// Only go trough if we know what it is
		if (interactionType != "tile" && interactionType != "creature") continue;

		// The interaction MUST have a name, no error checking for now...
		// TODO add error checking for this
		std::string interactionName = interactionNode->first_attribute("name")->value();

		// Look for all the actions
		for (xml_node<> *actionNode = interactionNode->first_node("action");
		actionNode;actionNode = actionNode->next_sibling("action")) {
			Interaction currentInteraction = impl.parseInteractions(actionNode);
			if (interactionType == "tile")
				re.tiles[interactionName].options.push_back(currentInteraction);
			else if (interactionType == "creature")
				re.creatures[interactionName].options.push_back(currentInteraction);
		}
	}
	delete[] newText;
	return re;

}

Interaction InteractionsParser::Impl::parseInteractions(xml_node<>* action)
{
	Interaction re;

	for (xml_attribute<> *actionAttribute = action->first_attribute();
	actionAttribute; actionAttribute = actionAttribute->next_attribute()) {

			if (strcmp(actionAttribute->name(), "name") == 0) {
				re.name = actionAttribute->value();
			}
			else if (strcmp(actionAttribute->name(), "instant") == 0) {
				re.instant = true;
			}
			else if (strcmp(actionAttribute->name(), "if") == 0) {
				re.ifConditional = actionAttribute->value();
			}
		}

	re.command = action->value();
	return re;
}
