#include "Mortal.h"
#include "Parser/Characters.h"
#include "ScriptLoader.h"

Mortal* Mortal::create(int classIndex, std::string playerName, int playerIndex) {
	Mortal* re = new (std::nothrow) Mortal();
	if (re && re->init(classIndex, playerName, playerIndex)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool Mortal::init(int classIndex, std::string playerName, int playerIndex) {
	CharacterParser::CharacterVector characters = CharacterParser::parseVector(ScriptLoader::loadXmlScript(ScriptLoader::XML_PLAYERS));
	m_playerIndex = playerIndex;
	auto characterClass = characters[classIndex];

	// init super first!
	Animal::init(characterClass.movements, SPRITE_PLAYER, characterClass.scores.at(ABILITY_SPEED));

	// Save the information for later use!
	m_name = playerName;
	m_className = characterClass.name;
	return true;
}

std::string Mortal::getPlayerName() {
	return m_name;
}

std::string Mortal::getClassName() {
	return m_className;
}

void Mortal::update(float delta) {
	Animal::update(delta);
}

int Mortal::getPlayerIndex() {
	return m_playerIndex;
}