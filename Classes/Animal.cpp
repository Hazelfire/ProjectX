#include "Animal.h"
#include "PathFinder.h"
#include "XTileMap.h"
#include "SpriteLoader.h"
#include "ResourceMacros.h"
#define DEFAULT_SPEED 3.0f


Animal* Animal::create(MovementDirections directions,SpriteType type, float speed) {
	Animal* re = new (std::nothrow) Animal();
	if (re && re->init(directions,type, speed)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

Animal* Animal::create(MovementDirections directions, SpriteType type) {
	return create(directions,type, DEFAULT_SPEED);
}

bool Animal::init(MovementDirections directions, SpriteType type, float speed) {
	m_directions = directions;
	m_speed = speed;
	m_spriteType = type;

	// default sprite
	m_currentSprite = SpriteLoader::loadSprite(directions.getSource(MovementDirections::IDLE, MovementDirections::LEFT), type);
	m_currentSprite->setAnchorPoint(Vec2(0, 0));

	addChild(m_currentSprite);
	setContentSize(m_currentSprite->getContentSize());
	setAnchorPoint(Vec2(0.5, (double)12 / getContentSize().height));

	return true;
}


void Animal::setMovementSpeed(float speed) {
	m_speed = speed;
}

float Animal::getMovementSpeed() {
	return m_speed;
}

void Animal::setTilePosition(Vec2i position) {
	setPosition(XTileMap::findWorldLocationAt(position));
}

Vec2i Animal::getTilePosition() {
	return XTileMap::findTileLocationAt(getPosition());
}

cocos2d::Vec2 Animal::getRealTilePosition() {
	return XTileMap::findTileLocationAt(getPosition());
}

// movement related code
//List of interactions
double Animal::moveOn(Vec2i position) {
	return moveTo(position, 0);
}

double Animal::moveTo(Vec2i position) {
	return moveTo(position, 1);
}

double Animal::moveTo(Vec2i position, int distance) {
	return moveTo(position, distance, getMovementSpeed());
}

double Animal::moveOn(Vec2i position, double speed) {
	return moveTo(position, 0, getMovementSpeed());
}

double Animal::moveTo(Vec2i position, int distance, double speed) {
	std::list<Vec2i> path = PathFinder::findPath(getTilePosition(), position, distance);
	if (path.empty()) return 0;
	Sequence* fullPath = PathFinder::findPathTo(getRealTilePosition(), position, distance, speed);

	double fullDuration;
	if (fullPath)
		fullDuration = PathFinder::findPathTo(getRealTilePosition(), position, distance, speed)->getDuration();
	else
		fullDuration = 0;

	// If the creature is already moving towards the tile specified
	if (!m_currentPath.empty() && path.back() == m_currentPath.back()) {
		return fullDuration;
	}
	stopAllActions();
	m_currentPath = path;

	MoveTo* moveTo = MoveTo::create(path.front().distance(getRealTilePosition()) / speed, XTileMap::findWorldLocationAt(path.front()));

	changeMovementSprite(path.front() - getTilePosition());
	m_currentDirection = path.front() - getTilePosition();
	m_movementTimeCount = 0;
	m_isMoving = true;
	m_currentMovementMovementSpeed = speed;
	m_movementDuration = moveTo->getDuration();

	stopAllActions();
	runAction(moveTo);
	return fullDuration;
}

void Animal::changeMovementSprite(Vec2i position) {
	retain();
	m_currentSprite->removeFromParentAndCleanup(true);
	m_currentSprite = SpriteLoader::loadSprite(m_directions.getMovementSource(position), m_spriteType);
	if (m_currentSprite && !SpriteLoader::isAnimate(m_directions.getMovementSource(position), m_spriteType)) {
		Sprite* segway = Sprite::create(SEGWAY_SPRITE);
		segway->getTexture()->setAliasTexParameters();
		segway->setAnchorPoint(Vec2(0.2, 0.2));
		m_currentSprite->addChild(segway);
	}
	addChild(m_currentSprite);
}

void Animal::changeIdleSprite(Vec2i position) {
	retain();
	m_currentSprite->removeFromParentAndCleanup(true);
	m_currentSprite = SpriteLoader::loadSprite(m_directions.getIdleSource(position), m_spriteType);
	addChild(m_currentSprite);
}

void Animal::makeNextMove() {

	Vec2i currentPosition = m_currentPath.front();

	// If we are on our last action
	if (m_currentPath.size() == 1) {
		changeIdleSprite(m_currentDirection);
		m_currentPath.pop_front();
		m_movementTimeCount = 0;
		m_isMoving = false;
		m_movementDuration = 0;
		return;
	}
	m_currentPath.pop_front();

	Vec2i nextDestination = m_currentPath.front();
	MoveTo* nextMove = MoveTo::create(nextDestination.distance(currentPosition) / m_currentMovementMovementSpeed, XTileMap::findWorldLocationAt(nextDestination));
	if (m_currentDirection != nextDestination - currentPosition) {
		m_currentDirection = nextDestination - currentPosition;
		changeMovementSprite(m_currentDirection);
	}

	m_movementTimeCount = 0;
	m_movementDuration = nextMove->getDuration();

	runAction(nextMove);
}

void Animal::update(float delta) {
	if (m_isMoving) {
		m_movementTimeCount += delta;
		if (m_movementTimeCount > m_movementDuration) {
			makeNextMove();
		}
	}

	Arena::getMapInstance()->setZOrderOfNode(getTilePosition(), this, 1);

}