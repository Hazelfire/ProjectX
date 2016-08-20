#include "PackageManagerScene.h"
#include "ResourceMacros.h"
#include "XBMPLabel.h"
#include "HelloWorldScene.h"
#include <Packages/PackageManager.h>
#include <Utils.h>

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

		EventListenerTouchOneByOne* touchListener = EventListenerTouchOneByOne::create();

		touchListener->onTouchBegan = [this, packageCardNode, packageIndex](Touch* touch, Event*) {
			Rect cardRect = Utils::getWorldRect(packageCardNode);
			if (cardRect.containsPoint(touch->getLocation())) {
				createPackageViewer(packageIndex);
				return true;
			}
			return false;
		};

		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, packageCardNode);
	}

	selectorNode->setPosition(20, visibleSize.height - 50);

	addChild(selectorNode);
}

void PackageManagerScene::createPackageViewer(int packageIndex) {
	Node* oldViewer = getChildByName("viewer");
	if (oldViewer) {
		oldViewer->removeFromParentAndCleanup(true);
	}


	Size visibleSize = Director::getInstance()->getVisibleSize();

	Node* viewer = Node::create();
	viewer->setName("viewer");
	viewer->setContentSize(Size((visibleSize.width / 2) - 20, visibleSize.height - 50));
	viewer->setAnchorPoint(Vec2(1, 1));
	viewer->setPosition(visibleSize.width + 20, visibleSize.height - 50);

	// Getting requested package
	auto packagesInfo = PackageManager::getInstance()->getPackageInfo();

	auto packageIter = packagesInfo.begin();
	for (int i = 0; i < packageIndex; i++)
		packageIter++;

	PackageManager::PackageInfo package = *packageIter;


	// Icon
	Sprite* packageIcon;
	if (package.icon.empty()) {
		packageIcon = Sprite::create(DEFAULT_PACKAGE_ICON);
	}
	else {
		packageIcon = Sprite::create(package.icon);
	}
	packageIcon->setScale((viewer->getContentSize().width / 4) / packageIcon->getContentSize().width, (viewer->getContentSize().width / 4) / packageIcon->getContentSize().height);
	packageIcon->setAnchorPoint(Vec2(0, 1));
	packageIcon->setPosition(Vec2(0, viewer->getContentSize().height));
	viewer->addChild(packageIcon);

	// Name
	XBMPLabel* nameLabel = XBMPLabel::create(package.name, "Pixelfont", 40, XBMPLabel::LEFT);
	Vec2 nameLabelPosition = Vec2((viewer->getContentSize().width / 4) + 10, viewer->getContentSize().height - nameLabel->getContentSize().height);
	nameLabel->setPosition(nameLabelPosition);
	viewer->addChild(nameLabel);

	// Author
	XBMPLabel* authorLabel = XBMPLabel::create(package.author, "Pixelfont", 40, XBMPLabel::LEFT);
	authorLabel->setPosition(nameLabelPosition + Vec2(0, -40));
	viewer->addChild(authorLabel);

	// Version
	XBMPLabel* versionLabel = XBMPLabel::create(package.version, "Pixelfont", 40, XBMPLabel::LEFT);
	versionLabel->setPosition(nameLabelPosition + Vec2(0, -80));
	viewer->addChild(versionLabel);

	// Folder
	XBMPLabel* folderLabel = XBMPLabel::create("folder: " + package.folder, "Pixelfont", 40, XBMPLabel::LEFT);
	folderLabel->setPosition(nameLabelPosition + Vec2(0, -120));
	viewer->addChild(folderLabel);

	addChild(viewer);

}