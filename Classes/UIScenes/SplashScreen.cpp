#include "SplashScreen.h"
#include "../ResourceMacros.h"
#include "HelloWorldScene.h"
#include "Save.h"
#include "Debug.h"
#include "DownloadBasePackageScene.h"

static const std::string splashes[] = {
	{PROJECTX_SPLASH},
	{ COCOS2DX_SPLASH },
	{""}
};

using namespace cocos2d;

SplashScreen* SplashScreen::create() {
	SplashScreen* re = new (std::nothrow) SplashScreen();
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

bool SplashScreen::init() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vector<FiniteTimeAction*> actionsVector;

	if (!Save::read(&SaveInformation::skipSplashes)) {

		Debugger::log("Displaying splashes", DEBUG_GENERIC);

		int i = 0;
		while (!splashes[i].empty()) {
			Sprite* currentSplash = Sprite::create(splashes[i]);

			if (!currentSplash) { // If splash file not found
				Debugger::logWarning("Could not find splash image file for: " + splashes[i], DEBUG_GENERIC);
				continue;
			}

			currentSplash->retain();
			currentSplash->setPosition(origin + (visibleSize / 2));
			currentSplash->setScale((visibleSize.width / 2) / currentSplash->getContentSize().width);
			// fade in
			actionsVector.pushBack(CallFunc::create([this, currentSplash]() {
				this->addChild(currentSplash);
				currentSplash->setOpacity(0);
				currentSplash->runAction(FadeIn::create(1));
			}));

			actionsVector.pushBack(DelayTime::create(4));

			// fade out
			actionsVector.pushBack(CallFunc::create([this, currentSplash]() {
				currentSplash->runAction(FadeOut::create(1));
			}));
			actionsVector.pushBack(DelayTime::create(1));

			// destroy
			actionsVector.pushBack(CallFunc::create([currentSplash]() {
				currentSplash->removeFromParent();
			}));

			i++;
		}
	}
	else {
		Debugger::log("Skipped Splashes", DEBUG_GENERIC);
	}
	// Check if we have the base package
	if (!cocos2d::FileUtils::getInstance()->isDirectoryExist("pack/Base")) {
		actionsVector.pushBack(CallFunc::create([this]() {
			Director::getInstance()->replaceScene(DownloadBasePackageScene::create());
		}));
	}
	else {
		actionsVector.pushBack(CallFunc::create([this]() {
			Director::getInstance()->replaceScene(TransitionFade::create(1, HelloWorld::createScene()));
		}));
	}
	
		Sequence* splashSequence = Sequence::create(actionsVector);
		runAction(splashSequence);
	
	return true;
}