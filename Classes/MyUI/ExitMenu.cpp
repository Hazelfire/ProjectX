#include "ExitMenu.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "Arena.h"
#include "Dialougue.h"
#include "Utils.h"

using namespace cocos2d;

ExitMenu* ExitMenu::create() {
	ExitMenu* re = new ExitMenu();
	if (re && re->init()) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
};

bool ExitMenu::init() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Sprite* background = Sprite::create(EXIT_MENU_BACKGROUND);
	background->setScale((visibleSize.width / 5) / background->getContentSize().width, (visibleSize.height / 2) / background->getContentSize().height);
	Size backgroundSize = Size(background->getContentSize().width * background->getScaleX(), background->getContentSize().height * background->getScaleY());
	background->setPosition(backgroundSize / 2);
	background->setAnchorPoint(Vec2(0.5, 0.5));
	addChild(background);

	XBMPLabel* exitLabel = XBMPLabel::create("Exit Game", "Pixelfont", 100, XBMPLabel::CENTER);
	exitLabel->setPosition(background->getContentSize().width / 2, background->getContentSize().height - exitLabel->getContentSize().height);
	exitLabel->setCallback([]() {
		Dialogue::promptPlayer("Are you sure?", [](bool result) {
			if (result) Arena::endGame();
		}
		, Director::getInstance()->getRunningScene());
	});

	// Touch checks
	EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		Rect backgroundRect = Utils::getWorldRect(this);

		if (backgroundRect.containsPoint(touch->getLocation())) {
			// You clicked inside the box, swallow
			return true;
		}
		else {
			removeFromParentAndCleanup(true);
			return true;
		}
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	background->addChild(exitLabel);

	setContentSize(backgroundSize);
	setAnchorPoint(Vec2(0.5, 0.5));
	return true;
}