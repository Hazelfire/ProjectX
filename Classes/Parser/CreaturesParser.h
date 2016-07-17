#include <string>
#include <list>
#include <unordered_map>
#include "MovementDirection.hpp"

#define DEFAULT_CREATURE_SPEED 0.3

struct CreatureInteraction {
	std::string name;
	std::string command;
	std::string conditional;
};

struct CreatureInformation {
	std::list<std::string> tags;
	std::string ai;
	MovementDirections movements;
	float movementSpeed = DEFAULT_CREATURE_SPEED;
	std::list<CreatureInteraction> interactions;
};

struct CreatureList {
	std::unordered_map<std::string ,CreatureInformation> creatures;
};

class CreatureParser {
public:
	static CreatureList parse(std::string fileName);
private:
	struct Impl;
	static Impl impl;
};