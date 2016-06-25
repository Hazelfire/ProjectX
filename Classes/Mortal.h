#pragma once
#include "Animal.h"

// Mortal class, remember to call update appropriately!

class Mortal : public Animal {
public:

	static Mortal* create(int classIndex, std::string playerName, int playerIndex);
	
	std::string getPlayerName();
	std::string getClassName();
	int getPlayerIndex();
protected:
	void update(float delta);
	bool init(int classIndex, std::string playerName, int playerIndex);
private:
	int m_playerIndex;
	std::string m_name;
	std::string m_className;
};