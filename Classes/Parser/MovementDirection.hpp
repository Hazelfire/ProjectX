#pragma once
#include "../MyMath.h"
#include "rapidxml.hpp"
#include "StringOperations.h"

#define PI 3.141592653
#define RETURN_FALLBACK(__ITEM__,__FALLBACK__) do{if(__ITEM__.empty()){return __FALLBACK__;} else {return __ITEM__;}}while(0)

struct MovementDirections {
private:
	// All 16 different sprite combinations, which could all be animated. If someone could make 16 animations for the same character they would be legendary
	std::string m_left;
	std::string m_right;
	std::string m_up;
	std::string m_down;
	std::string m_upRight;
	std::string m_upLeft;
	std::string m_downRight;
	std::string m_downLeft;
	std::string m_leftIdle;
	std::string m_rightIdle;
	std::string m_upIdle;
	std::string m_downIdle;
	std::string m_upRightIdle;
	std::string m_upLeftIdle;
	std::string m_downRightIdle;
	std::string m_downLeftIdle;
public:
	enum MovementType {
		IDLE,
		MOVING
	};

	enum MovementDirection {
		LEFT,
		UP_LEFT,
		UP,
		UP_RIGHT,
		RIGHT,
		DOWN_RIGHT,
		DOWN,
		DOWN_LEFT
	};
	// Initializes MovementDirections with a rapidxml node
	void createWithNode(rapidxml::xml_node<>* movementNode) {
		if (!movementNode) return;
		if (strcmp(movementNode->name(), "movement") != 0) return;

		for (rapidxml::xml_node<>* movementDirectionNode = movementNode->first_node(); movementDirectionNode; movementDirectionNode = movementDirectionNode->next_sibling()) {

#define movementIs(_name_) (strcmp(movementDirectionNode->name(), _name_) == 0)

			if (movementIs("left")) {
				m_left = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("right")) {
				m_right = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("up")) {
				m_up = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("down")) {
				m_down = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("upRight")) {
				m_upRight = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("upLeft")) {
				m_upLeft = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("downRight")) {
				m_downRight = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("downLeft")) {
				m_downLeft = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("leftIdle")) {
				m_leftIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("rightIdle")) {
				m_rightIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("upIdle")) {
				m_upIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("downIdle")) {
				m_downIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("upLeftIdle")) {
				m_upLeftIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("upRightIdle")) {
				m_upRightIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("downRightIdle")) {
				m_downRightIdle = StringOps::trim(movementDirectionNode->value());
			}
			else if (movementIs("downLeftIdle")) {
				m_downLeftIdle = StringOps::trim(movementDirectionNode->value());
			}
		}
	}

	MovementDirections() {}

	MovementDirections(rapidxml::xml_node<>* movementNode) { createWithNode(movementNode); }

	// gets the movement source based on a certain direction
	std::string getMovementSource(Vec2f direction) {
		double angle = direction.absoluteAngle();
		angle += PI / 16;
		double percentage = angle / (2 * PI);

		int movementIndex = (int)(percentage * 8);
		switch (movementIndex) {
		case 0:
			RETURN_FALLBACK(m_downRight, m_right);
		case 1:
			RETURN_FALLBACK(m_down, m_right);
		case 2:
			RETURN_FALLBACK(m_downLeft, m_left);
		case 3:
			RETURN_FALLBACK(m_left, m_left);
		case 4:
			RETURN_FALLBACK(m_upLeft, m_left);
		case 5:
			RETURN_FALLBACK(m_up, m_left);
		case 6:
			RETURN_FALLBACK(m_upRight, m_right);
		case 7:
			RETURN_FALLBACK(m_right, m_right);
		}
		return std::string();
	}

	// gets the idle source based on a certain direction
	std::string getIdleSource(Vec2f direction) {
		{
			double angle = direction.absoluteAngle();
			angle += fmod(PI / 16, 2*PI);

			double percentage = angle / (2 * PI);

			int movementIndex = (int)(percentage * 8);
			switch (movementIndex) {
			case 0:
				RETURN_FALLBACK(m_downRightIdle, m_rightIdle);
			case 1:
				RETURN_FALLBACK(m_downIdle, m_rightIdle);
			case 2:
				RETURN_FALLBACK(m_downLeftIdle, m_leftIdle);
			case 3:
				RETURN_FALLBACK(m_leftIdle, m_leftIdle);
			case 4:
				RETURN_FALLBACK(m_upLeftIdle, m_leftIdle);
			case 5:
				RETURN_FALLBACK(m_upIdle, m_leftIdle);
			case 6:
				RETURN_FALLBACK(m_upRightIdle, m_rightIdle);
			case 7:
				RETURN_FALLBACK(m_rightIdle, m_rightIdle);
			}
			return std::string();
		}
	}
	
	std::string getSource(MovementType type, MovementDirection direction) {
		switch (type) {
		case MOVING:
			switch (direction) {
			case LEFT:
				return m_left;
			case UP_LEFT:
				return m_upLeft;
			case UP:
				return m_up;
			case UP_RIGHT:
				return m_upRight;
			case RIGHT:
				return m_right;
			case DOWN_RIGHT:
				return m_downRight;
			case DOWN:
				return m_down;
			case DOWN_LEFT:
				return m_downLeft;
			}
		case IDLE:
			switch (direction) {
			case LEFT:
				return m_leftIdle;
			case UP_LEFT:
				return m_upLeftIdle;
			case UP:
				return m_upIdle;
			case UP_RIGHT:
				return m_upRightIdle;
			case RIGHT:
				return m_rightIdle;
			case DOWN_RIGHT:
				return m_downRightIdle;
			case DOWN:
				return m_downIdle;
			case DOWN_LEFT:
				return m_downLeftIdle;
			}
		}
	}
};