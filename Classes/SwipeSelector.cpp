#include "SwipeSelector.h"
#include "MyMath.h"
#include "Utils.h"

#define WINTOUCH true

using namespace cocos2d;
SwipeSelector* SwipeSelector::create(NodeVector nodeVector, Size size) {
	SwipeSelector* pRet = new (std::nothrow) SwipeSelector();
	if (pRet && pRet->init(nodeVector, size)) {
		pRet->autorelease();
		return pRet;
	}
	else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool SwipeSelector::init(NodeVector nodeVector, cocos2d::Size size) {

	// Makes sure that garbage collection does not collect these nodes
	for (NodeVector::iterator currentNode = nodeVector.begin(); currentNode != nodeVector.end(); currentNode++) {
		(*currentNode)->retain();
	}

	m_optionNodes = nodeVector;

	// Clipped viewport
	Size viewportSize = size;
	ClippingRectangleNode* clippedViewport = ClippingRectangleNode::create(Rect(Vec2::ZERO, viewportSize));
	clippedViewport->setContentSize(viewportSize);
	clippedViewport->setAnchorPoint(Vec2(0, 0));

	// The character viewer which can be dragged right and left or simply moved
	Node* characterViewerParent = Node::create();
	characterViewerParent->setPosition(viewportSize / 2);
	clippedViewport->addChild(characterViewerParent);

	m_viewer = Node::create();
	m_viewer->setContentSize(viewportSize);
	characterViewerParent->addChild(m_viewer);

	// The first character to be added
	m_viewer->addChild(m_optionNodes.at(0));

	setContentSize(viewportSize);
	setAnchorPoint(Vec2(0, 0));
	addChild(clippedViewport);

	EventListenerKeyboard* keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(SwipeSelector::keyPressedEvent, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

	// touch controls for mobile
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || WINTOUCH)
	EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(SwipeSelector::touchBeganEvent, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(SwipeSelector::touchMovedEvent, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(SwipeSelector::touchEndedEvent, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(SwipeSelector::touchEndedEvent, this);

	touchListener->setSwallowTouches(true);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, clippedViewport);
#endif


#define The_Truest_Of_Trues true
	return The_Truest_Of_Trues;
}

void SwipeSelector::keyPressedEvent(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*) {
	if (keycode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW || keycode == EventKeyboard::KeyCode::KEY_D) {
		right();
	}
	else if (keycode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || keycode == EventKeyboard::KeyCode::KEY_A) {
		left();
	}
}

void SwipeSelector::right() {
	m_viewer->stopAllActions();
	m_viewer->removeAllChildren();
	Size viewerSize = m_viewer->getContentSize();


	Node* oldCharacter = m_optionNodes.at(myMod(m_selectedItemIndex, (int)m_optionNodes.size()));
	oldCharacter->setPosition(Vec2(m_selectedItemIndex * viewerSize.width, 0));
	m_viewer->addChild(oldCharacter);

	double moveIndex = -(++m_selectedItemIndex);

	Node* newCharacter = m_optionNodes.at(myMod(m_selectedItemIndex, (int)m_optionNodes.size()));

	newCharacter->setPosition(Vec2(m_selectedItemIndex * viewerSize.width, 0));
	m_viewer->addChild(newCharacter);
	Vec2 characterViewerPos = m_viewer->getPosition();
	MoveTo* move = MoveTo::create(0.2, Vec2(moveIndex *viewerSize.width, characterViewerPos.y));
	EaseOut* easeOut = EaseOut::create(move->clone(), 4);
	m_viewer->runAction(easeOut);

	if (m_changeCallback != nullptr)
		m_changeCallback(getSelectedItemIndex());
}

void SwipeSelector::left() {
	m_viewer->stopAllActions();
	m_viewer->removeAllChildren();
	Size viewerSize = m_viewer->getContentSize();


	Node* oldCharacter = m_optionNodes.at(myMod(m_selectedItemIndex, (int)m_optionNodes.size()));
	oldCharacter->setPosition(Vec2(m_selectedItemIndex * viewerSize.width, 0));
	m_viewer->addChild(oldCharacter);

	double moveIndex = -(--m_selectedItemIndex);

	Node* newCharacter = m_optionNodes.at(myMod(m_selectedItemIndex, (int)m_optionNodes.size()));

	newCharacter->setPosition(Vec2(m_selectedItemIndex * viewerSize.width, 0));
	m_viewer->addChild(newCharacter);
	Vec2 characterViewerPos = m_viewer->getPosition();
	MoveTo* move = MoveTo::create(0.2, Vec2(moveIndex *viewerSize.width, characterViewerPos.y));
	EaseOut* easeOut = EaseOut::create(move->clone(), 4);
	m_viewer->runAction(easeOut);

	if (m_changeCallback != nullptr)
		m_changeCallback(getSelectedItemIndex());
}

void SwipeSelector::setChangeCallback(std::function<void(int)> callback) {
	m_changeCallback = callback;
}

int SwipeSelector::getSelectedItemIndex() {
	return myMod(m_selectedItemIndex, (int)m_optionNodes.size());
}

bool SwipeSelector::touchBeganEvent(cocos2d::Touch *touch, cocos2d::Event *event) {
	Rect worldRect = Utils::getWorldRect(this);
	if (worldRect.containsPoint(touch->getLocation())) {
		m_initialTouchPosition = touch->getLocation();
		m_currentTouchPosition = touch->getLocation();

		m_isTouching = true;

		return true;
	}
	return false;
}

void SwipeSelector::touchMovedEvent(cocos2d::Touch *touch, cocos2d::Event *event) {
	if (m_isTouching) {
		m_currentTouchPosition = touch->getLocation();

		// Checking if a swupe is complete
		Size visibleSize = Director::getInstance()->getVisibleSize();
		if (m_initialTouchPosition.x - m_currentTouchPosition.x > visibleSize.width * 0.05)
		{
			right();
			m_isTouching = false;

		}
		else if (m_initialTouchPosition.x - m_currentTouchPosition.x < -visibleSize.width * 0.08)
		{
			left();
			m_isTouching = false;

		}
	}
}

void SwipeSelector::touchEndedEvent(cocos2d::Touch *touch, cocos2d::Event *event) {
	// If the touch has not resulted in a swipe
	if (m_isTouching)
		m_touchCallback(getSelectedItemIndex());
	m_isTouching = false;
}

void SwipeSelector::setTouchCallback(std::function<void(int)> callback) {
	m_touchCallback = callback;
}