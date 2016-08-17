#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "PlayerSelector.h"
#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include "Save.h"
#include "MyUI/Dialougue.h"
#include "OptionsMenu.h"
#include "Debug.h"

USING_NS_CC;

#define TEXT_SIZE 40

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	Debugger::log("Starting menu screen", DEBUG_GENERIC);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Super Init
    if ( !Layer::init() )
    {
        return false;
    }

	// Background placement
	Sprite* background = Sprite::create(TITLE_BACKGROUND_IMAGE);
	if (background) {
		background->setScale(std::max(visibleSize.height / background->getContentSize().height, visibleSize.width / background->getContentSize().width));
		background->setAnchorPoint(Vec2(0, 0));
		background->setPosition(origin);
		addChild(background);

		// background panning
		Vector<FiniteTimeAction*> actionVector;
		if (visibleSize.height / background->getContentSize().height > visibleSize.width / background->getContentSize().width) {
			double distance = (background->getScale() * background->getContentSize().width) - visibleSize.width;
			MoveBy* moveBy = MoveBy::create(10, Vec2(-distance, 0));
			EaseInOut* ease = EaseInOut::create(moveBy, 2);
			actionVector.pushBack(ease);
			actionVector.pushBack(ease->reverse());
		}
		else {
			double distance = (background->getScale() * background->getContentSize().height) - visibleSize.height;
			MoveBy* moveBy = MoveBy::create(10, Vec2(0, -distance));
			EaseInOut* ease = EaseInOut::create(moveBy, 2);
			actionVector.pushBack(ease);
			actionVector.pushBack(ease->reverse());
		}
		Sequence* sequence = Sequence::create(actionVector);
		background->runAction(RepeatForever::create(sequence));
	}
	else {
		Debugger::logWarning(std::string("Failed to load background at: ") + TITLE_BACKGROUND_IMAGE, DEBUG_GENERIC);
	}

	// Title text placement
	Sprite* title = Sprite::create(TITLE_TEXT_IMAGE);
	if (title) {
		title->getTexture()->setAliasTexParameters();
		title->setScale(std::min(visibleSize.height / title->getContentSize().height, visibleSize.width / title->getContentSize().width));
		title->setAnchorPoint(Vec2(0, 0));
		title->setPosition(origin);
		addChild(title);
	}
	else {
		Debugger::logWarning(std::string("Failed to load title image at: ") + TITLE_TEXT_IMAGE, DEBUG_GENERIC);
	}

	// Start normal game
	auto startGame = XBMPLabel::create("Start Game", "Pixelfont",TEXT_SIZE, XBMPLabel::LEFT);
	startGame->setCallback([]() {
		auto playerSelectionScene = PlayerSelection::create(GameMode::SINGLEPLAYER);
		if(playerSelectionScene)
			Director::getInstance()->replaceScene(TransitionFade::create(0.5,playerSelectionScene));
		
	});

	startGame->setPosition(origin.x + 10, origin.y + (visibleSize.height / 2) + TEXT_SIZE + (TEXT_SIZE / 2));

	addChild(startGame);

#if NOT_MOBILE
	// Hosting, but not for mobile
	XBMPLabel* hostGame = XBMPLabel::create("Host Game", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	hostGame->setCallback([]() {
		auto playerSelectionScene = PlayerSelection::create(GameMode::HOST_GAME);
		if(playerSelectionScene)
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, playerSelectionScene));
	});

	hostGame->setPosition(origin.x + 10, origin.y + (visibleSize.height / 2));
	addChild(hostGame);
#endif

	// Join game
	XBMPLabel* joinGame = XBMPLabel::create("Join Game", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	joinGame->setCallback([]() {
		auto playerSelectionScene = PlayerSelection::create(GameMode::JOIN_GAME);
		if(playerSelectionScene)
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, playerSelectionScene));
	});

	joinGame->setPosition(origin.x + 10, origin.y + (visibleSize.height / 2) -( TEXT_SIZE + (TEXT_SIZE / 2)));
	addChild(joinGame);

	// Options
	XBMPLabel* options = XBMPLabel::create("Options", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	options->setCallback([]() {
		Director::getInstance()->pushScene(TransitionFade::create(0.5, OptionsMenu::create()));
	});
	options->setPosition(origin.x + 10, origin.y + (visibleSize.height / 2) - 2*(TEXT_SIZE + (TEXT_SIZE / 2)));
	addChild(options);
	
	// Exit
	XBMPLabel* exit = XBMPLabel::create("Exit", "Pixelfont", TEXT_SIZE, XBMPLabel::LEFT);
	exit->setCallback([]() {
		Director::getInstance()->end();
	});
	exit->setPosition(origin.x + 10, origin.y + (visibleSize.height / 2) - 3 * (TEXT_SIZE + (TEXT_SIZE / 2)));
	addChild(exit);



	// Checking if the player has inputed a name
	std::string playerName = Save::read(&SaveInformation::playerName);

	// If not, ask the player for it's name before continuing
	if (playerName.empty()) {
		queryPlayerName();
	}


    return true;
}


void HelloWorld::queryPlayerName() {

	Dialogue::promptPlayer("I don't know your name, what is it?", "Player Name", [](std::string playerName) {
		Save::write(playerName, &SaveInformation::playerName);
	}, this);

}
