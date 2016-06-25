#include "OptionsMenu.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "Save.h"

using namespace cocos2d;

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)
#define TEXT_SIZE 40

OptionsMenu* OptionsMenu::create() {
	OptionsMenu* re = new (std::nothrow) OptionsMenu();
	if (re && re->init()) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool OptionsMenu::init() {

	using namespace cocos2d::ui;

	Sprite* background = Sprite::create(OPTIONS_BACKGROUND);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	background->setScale(FILL(background->getContentSize(), visibleSize));
	background->setPosition(visibleSize / 2);
	addChild(background);

	XBMPLabel* nameLabel = XBMPLabel::create("Player Name", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	nameLabel->setPosition(Vec2(0,visibleSize.height));
	nameLabel->setAnchorPoint(Vec2(0, 1));
	addChild(nameLabel);

	TextField* nameField = TextField::create("", "Arial.ttf", TEXT_SIZE);
	nameField->setString(Save::read(&SaveInformation::playerName));
	nameField->setPosition(Vec2(0, visibleSize.height - TEXT_SIZE));
	nameField->setTextHorizontalAlignment(TextHAlignment::LEFT);
	nameField->setAnchorPoint(Vec2(0,1));
	addChild(nameField);
	nameField->addEventListener(CC_CALLBACK_2(OptionsMenu::inputTextCallback, this));

	XBMPLabel* backLabel = XBMPLabel::create("Back", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	backLabel->setAnchorPoint(Vec2(0, 0));
	backLabel->setCallback([]() {
		Director::getInstance()->popScene();
	});
	backLabel->setPosition(Vec2(0, 0));
	addChild(backLabel);
	return true;
}

void OptionsMenu::inputTextCallback(cocos2d::Ref* reference, cocos2d::ui::TextField::EventType event) {
	using namespace cocos2d::ui;
	if (TextField::EventType::INSERT_TEXT == event) {
		TextField* field = (TextField*)reference;
		std::string text = field->getString();
		Save::write(text, &SaveInformation::playerName);
	}
}