#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <unordered_map>
#include "Parser/Characters.h"
#include "SwipeSelector.h"
#include "GameInformation.h"
#include "MapSelector.h"
USING_NS_CC;
/* 
	Player selection header. This entire class is made for the selection of the player. I have a strange feeling that it is going to get
	really large
*/

class PlayerSelection : public Scene{
public:

	// creates the player selector based on the flags
	static PlayerSelection* create(GameMode gameMode);
private:
	virtual bool init(GameMode gameMode);

	class DetailsPreviewer : public Node {
	public:
		static DetailsPreviewer* create(CharacterParser::CharacterList characterList);
		void setCharacter(int index);
	private:
		virtual bool init(CharacterParser::CharacterList characterList);

		int m_characterIndex = 0;
		std::vector<CharacterParser::CharacterClass> m_characterInformation;
		Sprite* m_background;
	};
	void updateDetails(int);

	void keyPressedEvent(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*);
	void touchedSelectorEvent(int selectedIndex);

	void startGame();
	SwipeSelector* m_characterSelector;
	DetailsPreviewer* m_detailsPreviewer;
	GameMode m_flag;
	MapSelector* m_mapSelector;
	CharacterParser::CharacterList m_characterList;
};