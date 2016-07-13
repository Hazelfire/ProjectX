#include "Player.h"
#include "Interactions.h"
#include "Arena.h"
#include "MusicEngine.h"
#include "ResourceMacros.h"
#include "PathFinder.h"

Player* Player::m_playerInstance;

Player* Player::create(int classIndex, std::string playerName, int playerIndex) {
	Player *pRet = new(std::nothrow)Player();
	if (pRet && pRet->init(classIndex, playerName, playerIndex))
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

bool Player::init(int classIndex, std::string playerName, int playerIndex) {

	Mortal::init(classIndex, playerName, playerIndex);

	configureControls();
	scheduleUpdate();

	// Camera Follow
	Arena::getCameraInstance()->runAction(Follow::create(this, Rect::ZERO));

	m_playerInstance = this;

	return true;
}

bool Player::TouchEvent(Touch* touch, Event* touchEvent) {
	if (Arena::getState() == 0) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		Vec2 center = visibleSize / 2;

		//Gets coordinates in world sense (keep in mind that this is not screen sense)
		Vec2 worldTouch = (((touch->getLocation() - origin) / PIXELATION - center / PIXELATION + getPosition()));
		Vec2 tileCoordinates = XTileMap::findTileLocationAt(worldTouch);

		if (tileCoordinates.x<0 || tileCoordinates.y<0 || tileCoordinates.x>Arena::getMapInstance()->getMapSize().x || tileCoordinates.y> Arena::getMapInstance()->getMapSize().y) return true; // if they clicked out of the map
		Interact::InteractMap(tileCoordinates);

		return true;
	}
	return false;
}

// creates the touch listener for movements
void Player::configureControls() {
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(Player::TouchEvent, this);
	listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void Player::update(float delta) {
	Vec2 tileLocation = XTileMap::findTileLocationAt(getPosition());
	
	Arena::getMapInstance()->updateChunks(tileLocation);

	Mortal::update(delta);

	Music::playRegion(tileLocation);

	// Calls the interactions update method (for interaction scheduling)
	Interact::update(delta);
}

Player* Player::getInstance() {
	return m_playerInstance;
}
