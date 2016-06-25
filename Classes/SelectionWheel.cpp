#include "SelectionWheel.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "Arena.h"

SelectionWheel* SelectionWheel::create(std::list<std::string> options) {
	SelectionWheel *pRet = new(std::nothrow)SelectionWheel();
	if (pRet && pRet->init(options))
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

bool SelectionWheel::init(std::list<std::string> options) {
	cocos2d::Sprite* background = cocos2d::Sprite::create(INTERACTIONS_SELECTOR);
	background->setOpacity(200);
	addChild(background);
	m_memberCount = options.size();
	int i = 0;
	static const int radius =40;
	for (std::list<std::string>::iterator currentOption = options.begin(); currentOption != options.end(); currentOption++) {
		XBMPLabel* label = XBMPLabel::create(*currentOption, "Pixelfont", 200);
		label->setPosition(Vec2(cos((((double)i / m_memberCount) + (0.5/m_memberCount)) * 2 * PI )* radius, sin((((double)i / m_memberCount) + 0.5/m_memberCount) * 2 * PI) * radius) + background->getContentSize() / 2);
		background->addChild(label);
		i++;
	}
	m_callbackFunctions.resize(m_memberCount);
	m_listener = EventListenerTouchOneByOne::create();
	m_listener->setSwallowTouches(true);
	m_listener->onTouchBegan =CC_CALLBACK_2(SelectionWheel::touchEvent, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_listener, this);
	return true;
}

void SelectionWheel::setSectionCallback(int index, std::function<void()> function) {
	m_callbackFunctions[index] = function;
}

void SelectionWheel::callCallback(int index) {
	m_callbackFunctions[index]();
}

bool SelectionWheel::touchEvent(cocos2d::Touch* touch, cocos2d::Event* e) {
	// already in world coordinates
	Vec2 selectorPosition = getPosition();
	Vec2 touchPosition = touch->getLocation();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 center = visibleSize / 2;

	//Gets coordinates in world sense (keep in mind that this is not screen sense)
	Vec2 worldTouch = (((touch->getLocation() - origin) / PIXELATION - center / PIXELATION + Player::getInstance()->getPosition()));

	if (selectorPosition.distance(worldTouch) > 28 && selectorPosition.distance(worldTouch) < 64) {
		Vec2f relativeTouchPosition = worldTouch - selectorPosition;
		double angle = relativeTouchPosition.absoluteAngle();
		double percentage = angle / (2 * PI);
		int index = (int)(percentage * m_memberCount);
		callCallback(index);
		return true;
	}
	return false;
}
void SelectionWheel::removeFromParentAndCleanup(bool state) {
	Director::getInstance()->getEventDispatcher()->removeEventListener(m_listener);
	Node::removeFromParentAndCleanup(state);
}
