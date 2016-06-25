#pragma once
#include <list>
#include <unordered_map>
#include <vector>
#include "MovementDirection.hpp"
#define ABILITY_SPEED 0


#define DEFAULT_SPEED 3

class CharacterParser {
public:
	struct CharacterClass {
		std::string name;
		std::string description;
		MovementDirections movements;
		std::vector<double> scores;
	};


	typedef std::list<CharacterClass> CharacterList;
	
	static CharacterList parse(std::string source);

	typedef std::vector<CharacterClass> CharacterVector;
	static CharacterVector parseVector(std::string source);
private:
	class Impl;
	static Impl impl;
};