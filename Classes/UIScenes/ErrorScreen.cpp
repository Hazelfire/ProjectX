#include "ErrorScreen.h"
#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include "HelloWorldScene.h"
using namespace cocos2d;

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)

ErrorScreen* ErrorScreen::create(std::string error) {
	ErrorScreen* re = new (std::nothrow) ErrorScreen();
	if (re && re->init(error)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool ErrorScreen::init(std::string message) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// placing Background
	Sprite* background = Sprite::create(ERROR_SCREEN_BACKGROUND);
	background->setScale(FILL(background->getContentSize(), visibleSize));
	background->setPosition(origin +(visibleSize / 2 ));
	addChild(background);

	XBMPLabel* errorLabel = XBMPLabel::create(message, "Pixelfont", 40, XBMPLabel::CENTER);
	errorLabel->setPosition(origin + (visibleSize / 2));
	addChild(errorLabel);

	EventListenerTouchOneByOne * touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [touchListener](Touch* touch, Event* event) {
		Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
		Director::getInstance()->replaceScene(HelloWorld::createScene());
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}