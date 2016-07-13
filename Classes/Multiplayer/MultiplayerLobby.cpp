#include "MultiplayerLobby.h"
#include "ResourceMacros.h"
#include "network/SocketIO.h"
#include "XBMPLabel.h"
#include "MyUI/Dialougue.h"
#include "SpriteLoader.h"
#include "Parser/Characters.h"
#include "ScriptLoader.h"

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)

#define TITLE_SIZE 50

MultiplayerLobby* MultiplayerLobby::create(GameInformation gameInfo, bool hostMode) {
	MultiplayerLobby* pRet = new (std::nothrow) MultiplayerLobby();
	if (pRet && pRet->init(gameInfo, hostMode)) {
		pRet->autorelease();
		return pRet;
	}
	else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool MultiplayerLobby::init(GameInformation gameInfo, bool hostMode) {
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Node* layer = Node::create();
	layer->setPosition(origin);
	addChild(layer);

	scheduleUpdate();

	Sprite* background = Sprite::create(LOBBY_BACKGROUND);
	background->setScale(FILL(background->getContentSize(), visibleSize));
	background->setPosition(visibleSize / 2);
	layer->addChild(background);

	XBMPLabel* statusLabel = XBMPLabel::create("Connecting", "Pixelfont", TITLE_SIZE, XBMPLabel::CENTER);
	statusLabel->setName("status");
	statusLabel->setPosition(visibleSize / 2);
	addChild(statusLabel);

	XClient::init(gameInfo.playerInfo, this);

	if (hostMode) {
#if NOT_MOBILE
		
		m_server = new XServer();

		m_server->init(gameInfo.playerInfo.playerName, gameInfo.mapName);

		m_server->start(44075);
		XClient::getInstance()->connect("localhost", DEFAULT_PORT);

		// start game keyboard listener
		EventListenerKeyboard* startGameListener = EventListenerKeyboard::create();
		startGameListener->onKeyPressed = CC_CALLBACK_2(MultiplayerLobby::onKeyPressed, this);
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(startGameListener, this);
#endif
	}
	else {
		Dialogue::promtPlayer("What is the ip of the host?", "Internet Address", CC_CALLBACK_1(MultiplayerLobby::onSubmitAddress, this), this);
	}
	return true;
}

void MultiplayerLobby::onSubmitAddress(std::string address) {
	XClient::getInstance()->connect(address, DEFAULT_PORT);
}

void MultiplayerLobby::updateClientList() {


	if (getChildByName("status"))
		getChildByName("status")->removeFromParent();

	Size visibleSize = Director::getInstance()->getVisibleSize();

	removeChildByName("Player List");
	Node* playerList = Node::create();
	playerList->setContentSize(Size(visibleSize.width / 4, visibleSize.height));
	playerList->setAnchorPoint(Vec2(0, 0));
	playerList->setName("Player List");


	static const int EntrySize = 30;

	XBMPLabel* clientListLabel = XBMPLabel::create("Client List:", "Pixelfont", EntrySize, XBMPLabel::LEFT);
	clientListLabel->setAnchorPoint(Vec2(0, 1));
	playerList->addChild(clientListLabel);

	int playerIndex = 0;
	for (PlayerList::iterator currentPlayer = m_connectedPlayers.begin(); currentPlayer != m_connectedPlayers.end(); currentPlayer++) {

		XBMPLabel* currentPlayerLabel = XBMPLabel::create(currentPlayer->playerName, "Pixelfont", EntrySize, XBMPLabel::LEFT);
		currentPlayerLabel->setAnchorPoint(Vec2(0, 1));
		currentPlayerLabel->setPosition(Vec2(0, -((playerIndex + 1) * EntrySize)));

		playerList->addChild(currentPlayerLabel);


		// Character class preview
		static CharacterParser::CharacterList characterList = CharacterParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_PLAYERS));

		CharacterParser::CharacterList::iterator currentCharacter = characterList.begin();

		// iterates through the list until we have our character
		for (int i = 0; i < currentPlayer->classIndex; i++)
			currentCharacter++;

		// gets the sprite needed
		Node* classSprite = SpriteLoader::loadSprite(currentCharacter->movements.getSource(MovementDirections::MOVING, MovementDirections::RIGHT), SPRITE_PLAYER);

		classSprite->setScale(EntrySize / classSprite->getContentSize().height);

		classSprite->setAnchorPoint(Vec2(1, 1));

		classSprite->setPosition(Vec2(playerList->getContentSize().width - 10, -((playerIndex + 1) * 30)));

		playerList->addChild(classSprite);

		playerIndex++;

	}

	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	playerList->setPosition(Vec2(visibleSize.width*(3.0 / 4), visibleSize.height) + origin);

	addChild(playerList);

}

void MultiplayerLobby::onKeyPressed(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*) {
	if (keycode == cocos2d::EventKeyboard::KeyCode::KEY_ENTER) {
		XClient::getInstance()->startGame();
	}
}

void MultiplayerLobby::onPlayerConnect(PlayerInformation playerInfo) {
	m_connectedPlayers.push_back(playerInfo);
	updateClientList();
}

void MultiplayerLobby::onPlayerDisconnect(int playerIndex) {
	auto disconnectedPlayer = m_connectedPlayers.begin();

	for (int i = 0; i < playerIndex; i++)
		disconnectedPlayer++;

	m_connectedPlayers.erase(disconnectedPlayer);
	updateClientList();
}

void MultiplayerLobby::onServerInformation(std::string hostName, std::string map) {
	
	if(getChildByName("status"))
		getChildByName("status")->removeFromParent();

	XBMPLabel* hostNameTitle = XBMPLabel::create(hostName + "'s game", "Pixelfont",TITLE_SIZE, XBMPLabel::CENTER);
	
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// Top-middle
	hostNameTitle->setPosition(Vec2(visibleSize.width/2, visibleSize.height-TITLE_SIZE));
	addChild(hostNameTitle);

#define MAP_TEXT_SIZE 30
	XBMPLabel* mapName = XBMPLabel::create(map, "Pixelfont", MAP_TEXT_SIZE, XBMPLabel::CENTER);
	mapName->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - TITLE_SIZE - (MAP_TEXT_SIZE * (3.0 / 2))));

	addChild(mapName);
}

void MultiplayerLobby::onError(std::string error) {
	if (getChildByName("status"))
		((XBMPLabel*)getChildByName("status"))->setString(error);
}