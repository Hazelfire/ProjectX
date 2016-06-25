#include "PlayerSelector.h"
#include "../Arena.h"
#include "ResourceMacros.h"
#include "ScriptLoader.h"
#include "Parser/Terrain.h"
#include "SpriteLoader.h"
#include "Multiplayer/MultiplayerLobby.h"
#include "Save.h"
#include "XBMPLabel.h"
#include "Debug.h"
#include "ErrorScreen.h"
#include "HelloWorldScene.h"

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)
#define MAX_PLAYER_HEIGHT 48.0

using namespace ui;
PlayerSelection* PlayerSelection::create(GameMode flag)
{
	PlayerSelection *pRet = new(std::nothrow)PlayerSelection();
	if (pRet && pRet->init(flag))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool PlayerSelection::init(GameMode flag) {

	m_flag = flag;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create(PLAYER_SELECTION_BACKGROUND);
	if (background) {
		background->setPosition(origin + Vec2(visibleSize / 2));
		background->setScale(FILL(background->getContentSize(), visibleSize));
		addChild(background);
	}
	else {
		Debugger::logError(std::string("Failed to load player selection background at: ") + PLAYER_SELECTION_BACKGROUND, DEBUG_PLAYER_SELECTION);
	}

	CharacterParser::CharacterList parsedCharacterList = CharacterParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_PLAYERS));
	// checks that we have players
	if (parsedCharacterList.empty()){
		Debugger::logError("Failed to load any character classes", DEBUG_PLAYER_SELECTION);
		Director::getInstance()->replaceScene(ErrorScreen::create("Could not load any characters"));
		return false;

	}
	m_characterList = parsedCharacterList;

	NodeVector vector;
	for (CharacterParser::CharacterList::iterator currentCharacter = m_characterList.begin(); currentCharacter != m_characterList.end(); currentCharacter++) {
		std::string movementSpriteName =  currentCharacter->movements.getSource(MovementDirections::IDLE, MovementDirections::LEFT);
		Node* movementSprite = SpriteLoader::loadSprite(movementSpriteName, SPRITE_PLAYER);
		movementSprite->setAnchorPoint(Vec2(0.5, 0.5));
		movementSprite->setScale(visibleSize.height / 48);
		vector.push_back(movementSprite);

	}
	m_characterSelector = SwipeSelector::create(vector, Size(visibleSize.width/2,visibleSize.height));
	m_characterSelector->setPosition(origin);

	m_detailsPreviewer = DetailsPreviewer::create(m_characterList);

	m_characterSelector->setChangeCallback(CC_CALLBACK_1(PlayerSelection::updateDetails, this));
	m_characterSelector->setTouchCallback(CC_CALLBACK_1(PlayerSelection::touchedSelectorEvent, this));

	m_detailsPreviewer->setPosition(origin + Vec2(visibleSize.width/2, 0));
	addChild(m_characterSelector);
	addChild(m_detailsPreviewer);

	EventListenerKeyboard* keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(PlayerSelection::keyPressedEvent, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

	return true;
}

void PlayerSelection::touchedSelectorEvent(int selectedIndex) {
	startGame();
}

void PlayerSelection::keyPressedEvent(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*) {
	if (keycode == EventKeyboard::KeyCode::KEY_ENTER) {
		startGame();
	}
	else if (keycode == EventKeyboard::KeyCode::KEY_ESCAPE){
			Director::getInstance()->replaceScene(cocos2d::TransitionFade::create(0.5, HelloWorld::createScene()));
	}
}

void PlayerSelection::updateDetails(int index) {
	m_detailsPreviewer->setCharacter(index);
}

void PlayerSelection::startGame() {
	GameInformation gameInfo;
	gameInfo.mapName = "standard";
	
	int playerIndex = m_characterSelector->getSelectedItemIndex();
	PlayerInformation playerInfo;
	playerInfo.playerName = Save::read(&SaveInformation::playerName);
	playerInfo.classIndex  = playerIndex;

	gameInfo.playerInfo = playerInfo;
	if (m_flag != GameMode::JOIN_GAME) {
		MapSelector* mapSelector = MapSelector::create(gameInfo, m_flag);
		if(mapSelector)
			Director::getInstance()->replaceScene(mapSelector);
		return;
	}
	else {
		MultiplayerLobby* lobby = MultiplayerLobby::create(gameInfo, false);
		Director::getInstance()->replaceScene(lobby);
	}
}

PlayerSelection::DetailsPreviewer* PlayerSelection::DetailsPreviewer::create(CharacterParser::CharacterList list) {
	DetailsPreviewer *pRet = new(std::nothrow)DetailsPreviewer();
	if (pRet && pRet->init(list))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool PlayerSelection::DetailsPreviewer::init(CharacterParser::CharacterList list) {

	setOpacity(100);

	m_characterInformation = { list.begin(), list.end() };

	Size visibleSize = Director::getInstance()->getVisibleSize();
	static const int padding = 20;
	Size previewerSize = Size((visibleSize.width/2)-(padding* 2), visibleSize.height - (padding *2));
	
	m_background = Sprite::create(DETAILS_PREVIEWER_BACKGROUND);
	m_background->setAnchorPoint(Vec2::ZERO);
	m_background->setScale(previewerSize.width/m_background->getContentSize().width, previewerSize.height/ m_background->getContentSize().height);
	m_background->setPosition(Vec2(padding, padding));
	addChild(m_background);

	setAnchorPoint(Vec2(0, 0));
	setCharacter(0);
	return true;
	
}

void PlayerSelection::DetailsPreviewer::setCharacter(int index) {
	CharacterParser::CharacterClass characterClass = m_characterInformation.at(index);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	m_background->removeAllChildren();

	XBMPLabel* titleLabel = XBMPLabel::create(characterClass.name, "Pixelfont",visibleSize.height/14,XBMPLabel::CENTER,100, 0);
	titleLabel->setAnchorPoint(Vec2::ZERO);
	titleLabel->setPosition(Vec2(m_background->getContentSize().width / 2, m_background->getContentSize().height - (titleLabel->getContentSize().height * titleLabel->getScale())));
	m_background->addChild(titleLabel);

	XBMPLabel* descriptionLabel = XBMPLabel::create(characterClass.description, "Pixelfont", visibleSize.height / 20, XBMPLabel::LEFT,100,0);
	descriptionLabel->setAnchorPoint(Vec2::ZERO);
	descriptionLabel->setPosition(Vec2(0, m_background->getContentSize().height - ((titleLabel->getContentSize().height * titleLabel->getScale()) + (descriptionLabel->getContentSize().height * descriptionLabel->getScale()))));
	m_background->addChild(descriptionLabel);

}
