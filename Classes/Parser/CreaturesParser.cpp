#include "CreaturesParser.h"
#include "XMLCommon.h"
#include "rapidxml.hpp"
#include <sstream>

using namespace rapidxml;

struct CreatureParser::Impl {
	static CreatureInformation parseCreature(rapidxml::xml_node<>* creatureNode);
};

CreatureList CreatureParser::parse(std::string source) {
	CreatureList re;

	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());

	xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);

	for (xml_node<>* creatureNode = doc.first_node("creature"); creatureNode; creatureNode = creatureNode->next_sibling()) {
		re.creatures[creatureNode->first_attribute("name")->value()] = impl.parseCreature(creatureNode);
	}
	delete[] newText;
	return re;

}

CreatureInformation CreatureParser::Impl::parseCreature(xml_node<>* creatureNode) {
	CreatureInformation re;
	re.ai = creatureNode->first_attribute("ai")->value();

	re.movements = MovementDirections(creatureNode->first_node("movement"));

	if (creatureNode->first_attribute("speed")) {
		re.movementSpeed = std::atof(creatureNode->first_attribute("speed")->value());
	}

	// Tag parsing
	if (creatureNode->first_attribute("tags")){
		std::stringstream tags(std::string(creatureNode->value()));
		std::string currentTag;
		while (std::getline(tags, currentTag, ';')) {
			if (StringOps::trim(currentTag).empty()) continue;
			re.tags.push_back(StringOps::trim(currentTag));
		}
	}


	if (creatureNode->first_node("interactions")) {
		
		xml_node<>* interactionsNode = creatureNode->first_node("interactions");
		for (xml_node<>* actionNode = interactionsNode->first_node("action"); actionNode; actionNode = actionNode->next_sibling()) {
			CreatureInteraction interaction;

			if (actionNode->first_attribute("name"))
				interaction.name = actionNode->first_attribute("name")->value();

			interaction.command = actionNode->value();

			if (actionNode->first_attribute("if"))
				interaction.conditional = actionNode->first_attribute("if")->value();
			re.interactions.push_back(interaction);
		}
	}

	return re;

}