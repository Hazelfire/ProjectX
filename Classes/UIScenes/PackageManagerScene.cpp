#include "PackageManagerScene.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "HelloWorldScene.h"
#include <Packages/PackageManager.h>

#define FILL(_ITEM_SIZE_, _FILL_SIZE_) std::max(_FILL_SIZE_.width / _ITEM_SIZE_.width, _FILL_SIZE_.height / _ITEM_SIZE_.height)

PackageManagerScene* PackageManagerScene::create() {
	PackageManagerScene* re = new (std::nothrow) PackageManagerScene();
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

bool PackageManagerScene::init() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* background = Sprite::create(PACKAGE_MANAGER_BACKGROUND);
	background->setScale(FILL(background->getContentSize(), visibleSize));
	background->setPosition(origin);
	background->setAnchorPoint(Vec2(0, 0));

	addChild(background);

	XBMPLabel* titleLabel = XBMPLabel::create("Package Manager", "Pixelfont", 50, XBMPLabel::CENTER);
	titleLabel->setPosition(Vec2(background->getContentSize().width / 2, background->getContentSize().height - titleLabel->getContentSize().height));

	background->addChild(titleLabel);

	EventListenerKeyboard* listener = EventListenerKeyboard::create();

	listener->onKeyPressed = [](EventKeyboard::KeyCode code, Event*) {
		if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
			Director::getInstance()->replaceScene(HelloWorld::createScene());
		}
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	createPackageSelector();

	return true;
}

void PackageManagerScene::createPackageSelector() {

	const Size visibleSize = Director::getInstance()->getVisibleSize();
	const float cardWidth = visibleSize.width / 2;
	const float cardHeight = cardWidth / 4;

	std::list<PackageManager::PackageInfo> packageInfo = PackageManager::getInstance()->getPackageInfo();

	Node* selectorNode = Node::create();
	int packageIndex = 0;
	for (auto package : packageInfo) {
		Node* packageCardNode = Node::create();

		Sprite* packageCardBackground = Sprite::create(PACKAGE_CARD_IMAGE);
		packageCardBackground->setAnchorPoint(Vec2(0, 0));
		packageCardBackground->setScale(cardWidth / packageCardBackground->getContentSize().width, cardHeight / packageCardBackground->getContentSize().height);
		packageCardNode->addChild(packageCardBackground);
		
		packageCardNode->setContentSize(Size(cardWidth, cardHeight));
		packageCardNode->setAnchorPoint(Vec2(0, 1));

		// Package icon
		Sprite* packageIcon;
		if (package.icon.empty()) {
			packageIcon = Sprite::create(DEFAULT_PACKAGE_ICON);
		}
		else {
			packageIcon = Sprite::create(package.icon);
		}

		packageIcon->setScale((cardHeight - 10) / packageIcon->getContentSize().width, (cardHeight - 10) / packageIcon->getContentSize().height);
		packageIcon->setAnchorPoint(Vec2(0, 0));
		packageIcon->setPosition(Vec2(5, 5));
		packageIcon->getTexture()->setAliasTexParameters();
		packageCardNode->addChild(packageIcon);

		// Package name
		XBMPLabel* packageNameLabel = XBMPLabel::create(package.name, "Pixelfont", 30, XBMPLabel::LEFT);
		packageNameLabel->setPosition(Vec2(cardHeight + 10, cardHeight * (3.0 / 4)));
		packageCardNode->addChild(packageNameLabel);

		// Package author
		XBMPLabel* packageAuthorLabel = XBMPLabel::create(package.author, "Pixelfont", 20, XBMPLabel::LEFT);
		packageAuthorLabel->setPosition(Vec2(cardHeight + 10, cardHeight * (1.0 / 4)));
		packageCardNode->addChild(packageAuthorLabel);
		
		packageCardNode->setPosition(0, -packageIndex * cardHeight);

		selectorNode->addChild(packageCardNode);
	}

	selectorNode->setPosition(20, visibleSize.height - 50);

	addChild(selectorNode);
}