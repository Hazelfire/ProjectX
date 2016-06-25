#pragma once
#include "cocos2d.h"
#include "Parser/MovementDirection.hpp"
#include "MyMath.h"
#include "SpriteTypes.h"

class Animal : public cocos2d::Node {
public:
	static Animal* create(MovementDirections directions,SpriteType type);
	static Animal* create(MovementDirections directions,SpriteType type, float speed);
	
	// movement speeds
	void setMovementSpeed(float speed);
	float getMovementSpeed();

	// Positions
	void setTilePosition(Vec2i position);
	Vec2i getTilePosition();
	cocos2d::Vec2 getRealTilePosition();

	//List of different moves, return time needed to complete move
	virtual double moveOn(Vec2i position);
	virtual double moveOn(Vec2i position, double speed);
	virtual double moveTo(Vec2i position, int distance, double speed);
	virtual double moveTo(Vec2i position, int distance);
	virtual double moveTo(Vec2i position);


protected:
	virtual bool init(MovementDirections directions, SpriteType type, float speed);

	virtual void update(float delta);
private:

	// Animal information
	MovementDirections m_directions;
	float m_speed;

	// display sprite
	cocos2d::Node* m_currentSprite;

	// movement variables
	bool m_isMoving;
	double m_movementTimeCount;
	double m_movementDuration;
	std::list<Vec2i> m_currentPath;
	Vec2i m_currentDirection;
	double m_currentMovementMovementSpeed;

	// Movement functions
	void makeNextMove();

	// changing sprites
	void changeMovementSprite(Vec2i position);
	void changeIdleSprite(Vec2i position);

	SpriteType m_spriteType;
};