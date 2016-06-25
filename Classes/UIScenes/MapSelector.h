#pragma once
#include "cocos2d.h"
#include "GameInformation.h"
using namespace cocos2d;
class MapSelector : public Scene {
public:
	// Creates a Map selector scene from already generated game Inforamtion and Game Mode
	static MapSelector* create(GameInformation gameInfo, GameMode mode);

private:

	
	bool init(GameInformation gameInfo, GameMode mode);
	void startGame(int index);

	GameInformation m_gameInfo;
	GameMode m_gameMode;
};