#include "MapSelector.h"
#include "SwipeSelector.h"
#include "XBMPLabel.h"
#include "Parser/Terrain.h"
#include "ScriptLoader.h"
#include "../Arena.h"
#include "Multiplayer/MultiplayerLobby.h"
#include  "ResourceMacros.h"
#include "Debug.h"
#include "ErrorScreen.h"
#include "PlayerSelector.h"

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)

// Creates a pointer fo all!!
MapSelector* MapSelector::create(GameInformation gameInfo, GameMode mode) {
	MapSelector* re = new (std::nothrow) MapSelector();
	if (re && re->init(gameInfo, mode)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool MapSelector::init(GameInformation gameInfo, GameMode mode) {
	if (!Scene::init()) {
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	Sprite* background = Sprite::create(MAP_SELECTION_BACKGROUND);
	if (background) {
		background->setScale(FILL(background->getContentSize(), visibleSize));
		background->setPosition(origin + visibleSize / 2);
		addChild(background);
	}
	else {
		Debugger::logError("Failed to load map selection background", DEBUG_MAP_SELECTION);
	}

	TerrainData mapInfo = TerrainParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_MAP));
	if (mapInfo.mapsSize == 0) {
		Debugger::logError("Failed to load any maps", DEBUG_MAP_SELECTION);
		Director::getInstance()->replaceScene(ErrorScreen::create("Failed to load any maps"));
		return false;
	}
	NodeVector nodeVector;
	// for every map preset
	for (int mapIndex = 0; mapIndex < mapInfo.mapsSize; mapIndex++) {

		// creates the map label
		XBMPLabel* currentMapLabel = XBMPLabel::create(mapInfo.maps[mapIndex].name, "Pixelfont",visibleSize.height/6, XBMPLabel::CENTER);
		currentMapLabel->setAnchorPoint(Vec2(0.5, 0.5));	

		nodeVector.push_back(currentMapLabel);
	}

	// creates a swipe selector for the entire screen
	SwipeSelector* swipeSelector = SwipeSelector::create(nodeVector, visibleSize);
	swipeSelector->setAnchorPoint(Vec2(0,0));
	swipeSelector->setPosition(origin);
	swipeSelector->setTouchCallback(CC_CALLBACK_1(MapSelector::startGame, this));
	addChild(swipeSelector);

	// Keeping parameters for future use (see start game)
	m_gameInfo = gameInfo;
	m_gameMode = mode;

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = [this,swipeSelector](EventKeyboard::KeyCode code, Event*) {
		if (code == EventKeyboard::KeyCode::KEY_ENTER) {
			startGame(swipeSelector->getSelectedItemIndex());
		}
		else if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
			Director::getInstance()->replaceScene(PlayerSelection::create(m_gameMode));
		}
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	// Quit button
	XBMPLabel* quitLabel = XBMPLabel::create("quit", "Pixelfont", 50, XBMPLabel::LEFT);
	quitLabel->setAnchorPoint(Vec2(0, 0));
	quitLabel->setPosition(origin);
	quitLabel->setCallback([this]() {
		Director::getInstance()->replaceScene(PlayerSelection::create(m_gameMode));
	});
	addChild(quitLabel);

	return true;
}

void MapSelector::startGame(int index) {

	TerrainData mapInfo = TerrainParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_MAP));
	std::string mapName = mapInfo.maps[index].name;

	m_gameInfo.mapName = mapName;

	switch (m_gameMode) {
	case GameMode::SINGLEPLAYER:
		Arena arena;
		arena.create(m_gameInfo);
		Director::getInstance()->replaceScene(arena.getArenaScene());
		break;
	case GameMode::HOST_GAME:
		MultiplayerLobby* lobby = MultiplayerLobby::create(m_gameInfo, true);
		Director::getInstance()->replaceScene(lobby);
		break;
	}
}