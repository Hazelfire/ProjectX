#pragma once
#include "cocos2d.h"
#include <unordered_map>
#include "Mortal.h"

USING_NS_CC;

struct AbilityScores {
	double speed = 3;
};

class Player : public Mortal{
public:
	static Player* create(int classIndex, std::string playerName, int playerIndex);

	static Player* getInstance();	
private:

	virtual bool init(int classIndex, std::string playerName, int playerIndex);
	virtual void update(float);
	bool TouchEvent(Touch* touch, Event* touchEvent);
	void configureControls();

	static Player* m_playerInstance;
};