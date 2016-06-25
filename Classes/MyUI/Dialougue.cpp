#include "Dialougue.h"
#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include "ui/CocosGUI.h"
using namespace cocos2d;

void Dialogue::promtPlayer(std::string message, std::string placeholder, std::function<void(std::string)> callback, Node* runningScene) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* background = Sprite::create(DIALOGUE_BACKGROUND);

	// Sets the size to be 80% of the screen's width and height
	background->setScale((visibleSize.width*0.8) / background->getContentSize().width, (visibleSize.height * 0.8) / background->getContentSize().height);

	// Places in center
	background->setPosition(origin + (visibleSize / 2));
	runningScene->addChild(background);

	XBMPLabel* label = XBMPLabel::create(message, "Pixelfont", 50, XBMPLabel::CENTER);

	// places it at the top of the background
	label->setPosition(Vec2(background->getContentSize().width / 2, (background->getContentSize().height) - (label->getContentSize().height* label->getScaleY())));
	background->addChild(label);

	// auto because you won't beleive what the type is :p
	auto textField = ui::TextField::create(placeholder, "Arial.ttf", 100);
	textField->setPosition(background->getContentSize() / 2);
	background->addChild(textField);

	// Closing the dialogue, saves then removes
	std::function<void()> closeDialogue =
		[background, textField, callback]() {
		callback(textField->getString());
		background->removeFromParent();
	};

	XBMPLabel* doneLabel = XBMPLabel::create("Done", "Pixelfont", 100, XBMPLabel::CENTER);
	doneLabel->setCallback(closeDialogue);

	doneLabel->setPosition(Vec2(background->getContentSize().width / 2, doneLabel->getContentSize().height * doneLabel->getScaleX()));

	background->addChild(doneLabel);
	EventListenerKeyboard* enterListener = EventListenerKeyboard::create();
	enterListener->onKeyPressed = [closeDialogue, enterListener](cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event) {
		if (keycode == EventKeyboard::KeyCode::KEY_ENTER) {
			Director::getInstance()->getEventDispatcher()->removeEventListener(enterListener);
			closeDialogue();
		}
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(enterListener, background);


}