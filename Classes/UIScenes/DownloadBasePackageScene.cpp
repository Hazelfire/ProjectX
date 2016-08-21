#include "DownloadBasePackageScene.h"
#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include <Packages/PackageManager.h>
#include "HelloWorldScene.h"
#include <boost/thread.hpp>
#include <sstream>

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)

DownloadBasePackageScene* DownloadBasePackageScene::create() {
	DownloadBasePackageScene* re = new (std::nothrow) DownloadBasePackageScene();
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

bool DownloadBasePackageScene::init() {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* background = Sprite::create(DOWNLOAD_BASE_PACKAGE_BACKGROUND);
	background->setScale(FILL(background->getContentSize(), visibleSize));
	background->setAnchorPoint(Vec2(0, 0));
	background->setPosition(origin);
	addChild(background);

	XBMPLabel* messageLabel = XBMPLabel::create("Download base package, please wait", "Pixelfont", 30, XBMPLabel::CENTER);
	messageLabel->setName("Status");
	messageLabel->setPosition(visibleSize / 2);
	addChild(messageLabel);

	XBMPLabel* progressLabel = XBMPLabel::create("", "Pixelfont", 30, XBMPLabel::CENTER);
	progressLabel->setName("Progress");
	progressLabel->setPosition((visibleSize / 2) - Size(0,50));
	addChild(progressLabel);

	Vector<FiniteTimeAction*> actionVector;

	PackageManager::getInstance()->lockDownloadBase();

	PackageManager::getInstance()->downloadPackageBase();

	scheduleUpdate();

	return true;

}

void DownloadBasePackageScene::update(float delta) {
	if (!PackageManager::getInstance()->isInstallingBase()) {
		Director::getInstance()->replaceScene(HelloWorld::createScene());
	}
	else {
		XBMPLabel* statusLabel = (XBMPLabel*)getChildByName("Status");
		XBMPLabel* progressLabel = (XBMPLabel*)getChildByName("Progress");
		auto progress = PackageManager::getInstance()->getBasePackageProgress();
		if (progress.first.totalObjects == 0) return;
		if (progress.first.totalObjects == progress.first.receivedObjects) {
			statusLabel->setString("Installing base package, please wait");
			int percentageDone = ((float)progress.second.completedSteps / progress.second.totalSteps) * 100;
			
			std::stringstream ss;
			ss << percentageDone;
			
			progressLabel->setString(ss.str() + "%");
		}
		else {
			statusLabel->setString("Downloading base package, please wait");
			int percentageDone = ((float)progress.first.receivedObjects / progress.first.totalObjects) * 100;

			std::stringstream ss;
			ss << percentageDone;

			progressLabel->setString(ss.str() + "%");
		}
	}
}

void DownloadBasePackageScene::startDownload() {
	
}