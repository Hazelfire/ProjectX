#include "Characters.h"
#include <sstream>
#include "XMLCommon.h"

CharacterParser::CharacterVector CharacterParser::parseVector(std::string source) {
	CharacterList list = parse(source);
	return{ list.begin(), list.end() };
}

class CharacterParser::Impl {
public:
	static std::vector<double> parseScores(rapidxml::xml_node<>* scoresNode);
};

CharacterParser::Impl CharacterParser::impl;

CharacterParser::CharacterList CharacterParser::parse(std::string source) {
	CharacterList re;
	
	//Worst possible way of turning a string into a char* (rapidxml can't take const)
	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());

	rapidxml::xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);
	
	for (rapidxml::xml_node<> *characterNode = doc.first_node();
	characterNode;characterNode = characterNode->next_sibling()) {
		CharacterClass currentCharacter;
		currentCharacter.name = characterNode->first_attribute("name")->value();
		if(characterNode->first_node("description"))
			currentCharacter.description = StringOps::trim(characterNode->first_node("description")->value());
		currentCharacter.movements = MovementDirections(characterNode->first_node("movement"));
		currentCharacter.scores = impl.parseScores(characterNode->first_node("abilities"));
		re.push_back(currentCharacter);
	}
	delete[] newText;
	return re;
}

std::vector<double> CharacterParser::Impl::parseScores(rapidxml::xml_node<>* scoresNode) {
	std::vector<double> re;
	re.resize(1);
	if (!scoresNode) {
		re[ABILITY_SPEED] = DEFAULT_SPEED;
		return re;
	}


	if (scoresNode->first_node("speed")) {
		re[ABILITY_SPEED] = std::atof(StringOps::trim(scoresNode->first_node()->value()).c_str());
	}
	else {
		re[ABILITY_SPEED] = DEFAULT_SPEED;
	}
	return re;
}