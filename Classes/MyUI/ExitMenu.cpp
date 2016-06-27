#include "ExitMenu.h"
#include "ResourceMacros.h"

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
}