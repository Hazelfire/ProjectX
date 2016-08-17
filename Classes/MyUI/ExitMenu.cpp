#include "ExitMenu.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "Arena.h"
#include "Dialougue.h"

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
	addChild(background);

	XBMPLabel* exitLabel = XBMPLabel::create("Exit Game", "Pixelfont", 100, XBMPLabel::CENTER);
	exitLabel->setPosition(background->getContentSize().width / 2, background->getContentSize().height - exitLabel->getContentSize().height);
	exitLabel->setCallback([]() {
		Dialogue::promptPlayer("Are you sure?", [](bool result) {
			if (result) Arena::endGame();
		}
		, Director::getInstance()->getRunningScene());
	});

	background->addChild(exitLabel);
	return true;
}