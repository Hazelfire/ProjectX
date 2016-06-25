#pragma once
#include "cocos2d.h"

typedef std::vector<cocos2d::Node*> NodeVector;

class SwipeSelector : public cocos2d::Node {
public:
	static SwipeSelector* create(NodeVector nodes, cocos2d::Size size);
	int getSelectedItemIndex();
	void setChangeCallback(std::function<void(int)> callback);
	void setTouchCallback(std::function<void(int)> calback);
private:

	virtual bool init(NodeVector, cocos2d::Size size);

	// Making the swipe selector go right and left
	void right();
	void left();

	// Events
	void keyPressedEvent(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*);
	bool touchBeganEvent(cocos2d::Touch *touch, cocos2d::Event *event);
	void touchMovedEvent(cocos2d::Touch *touch, cocos2d::Event *event);
	void touchEndedEvent(cocos2d::Touch *touch, cocos2d::Event *event);

	// Swipe variables
	cocos2d::Vec2 m_initialTouchPosition;
	cocos2d::Vec2 m_currentTouchPosition;
	bool m_isTouching;

	// Callbacks
	std::function<void(int)> m_touchCallback;
	std::function<void(int)> m_changeCallback;

	cocos2d::Node* m_viewer;
	int m_selectorSize;
	int m_selectedItemIndex =0;
	NodeVector m_optionNodes;
};