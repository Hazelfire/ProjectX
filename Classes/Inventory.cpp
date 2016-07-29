#include "Inventory.h"
#include "Arena.h"
#include "Parser/StringOperations.h"
#include "SpriteLoader.h"
#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include "Debug.h"
#include "Lua/LuaItemUses.h"

std::map<std::string,ItemProperties> Inventory::m_itemsOwned;
std::unordered_map<std::string, InventoryItem> Inventory::m_itemInformation;
Node* Inventory::m_inventoryMenu;

#define SMALL_RESOURCE_SIZE 16
#define ITEM_SIZE 32
#define COLOMN_COUNT 3

#define LOAD_ITEMS() if (m_itemInformation.empty()) { \
						m_itemInformation = ItemsParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_ITEMS)).items; \
					} 

Inventory::InventoryPreviewer* Inventory::m_inventoryPreviewer;
Inventory::InventorySelector* Inventory::m_inventorySelector;
Inventory::CraftingPreviewer* Inventory::m_craftingItemPreviewer;
Inventory::CraftingSelector* Inventory::m_craftingList;

//***************************************************************
// These functions here are made for basic inventory needs, including
// creating Inventory menu, taking and giving resources
// and various other functions. giving resources and the like

Node* Inventory::createInventoryMenu() {

	LOAD_ITEMS();
	Node* re =Node::create();

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Sprite* background = Sprite::create(INVENTORY_BACKGROUND_IMAGE);
	if (!background) {
		Debugger::logError("Could not load inventory background", DEBUG_INVENTORY);
	}
	background->setName("background");

	background->setScale(((double)visibleSize.width / background->getContentSize().width) / 1.3);
	re->setContentSize(background->getContentSize() * background->getScale());
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = Inventory::touchEvent;
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, background);
	re->addChild(background);

	/*Node* inventoryItemPreviewer = Node::create();
	Node* inventoryList = Node::create();*/
	m_inventoryPreviewer = InventoryPreviewer::create(background, "");
	m_inventoryPreviewer->setPosition(Vec2(0, background->getContentSize().height / 2));
	background->addChild(m_inventoryPreviewer);

	m_inventorySelector = InventorySelector::create(background);
	m_inventorySelector->setPosition(Vec2(0, 0));
	background->addChild(m_inventorySelector);

	m_craftingItemPreviewer = CraftingPreviewer::create(background, "");
	m_craftingItemPreviewer->setPosition(Vec2(background->getContentSize() / 2));
	background->addChild(m_craftingItemPreviewer);

	m_craftingList = CraftingSelector::create(background);
	m_craftingList->setPosition(Vec2(background->getContentSize().width / 2,0));
	background->addChild(m_craftingList);
	
	re->setVisible(false);
	re->setPosition(visibleSize / 2);
	m_inventoryMenu = re;
	return re;
}

void Inventory::giveItem(std::string itemName, int quantity) {
	if (m_itemInformation.find(itemName) == m_itemInformation.end()) {
		// There is no such object... just report the error.
		Debugger::logError("Tried to give/take non-existant item: " + itemName, DEBUG_ITEMS);
		return;
	}
	if (m_itemsOwned[itemName].quantity + quantity <= 0) {
		m_itemsOwned.erase(itemName); // If there are no more of the item left, remove it
	}
	else {
		m_itemsOwned[itemName].quantity += quantity;
	}
	reconstruct();
}

bool Inventory::touchEvent(Touch* touch, Event* touchEvent) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Node* background = m_inventoryMenu->getChildByName("background");
	Vec2 menuPosition = background->getPosition();
	Size menuSize = background->getContentSize() * background->getScale();
	Rect backgroundRect = Rect(menuPosition + (visibleSize / 2) - (menuSize / 2), menuSize);
	if (!backgroundRect.containsPoint(touch->getLocation())) {
		closeInventory();
		return true;
	}
	return false;
}

void Inventory::openInventory() {
	Node* parentNode = Arena::getUILayer();

	m_inventoryMenu->removeFromParentAndCleanup(true);
#ifdef INFINITE_INVENTORY_LOADING
	while (true) 
#endif
	m_inventoryMenu = createInventoryMenu();
	parentNode->addChild(m_inventoryMenu);
	m_inventoryMenu->setVisible(true);

	Arena::setState(MENU_STATE);
}

void Inventory::closeInventory() {
	m_inventoryMenu->setVisible(false);
	Arena::setState(GAME_STATE);
}

bool Inventory::canCraftItem(std::string itemName) {
	InventoryItem itemInformation = m_itemInformation[itemName];
	for (std::list<Recipe>::iterator currentRecipe = itemInformation.recipies.begin(); currentRecipe != itemInformation.recipies.end(); currentRecipe++) {
		if (satisfiesRecipe(*currentRecipe)) {
			return true;
		}
	}
	return false;
}

bool Inventory::satisfiesRecipe(Recipe recipe) {

	// For every requirement
	for (Recipe::iterator ingredientRequirement = recipe.begin(); ingredientRequirement != recipe.end(); ingredientRequirement++) {
		std::string resourceRequirement = ingredientRequirement->first;
		int quantityRequirement = ingredientRequirement->second;
		if (!hasItem(resourceRequirement, quantityRequirement)) {
			return false;
		}
	}
	return true;
}

std::list<Recipe>::iterator Inventory::getFirstCraftableRecipe(std::list<Recipe>& recipes) {
	for (std::list<Recipe>::iterator currentRecipe = recipes.begin(); currentRecipe != recipes.end(); currentRecipe++) {
		if (satisfiesRecipe(*currentRecipe))
			return currentRecipe;
	}
}

bool Inventory::hasItem(std::string itemName, int quantity) {
	// If the player does not have the item
	if (m_itemsOwned.find(itemName) == m_itemsOwned.end()) return false;

	// If the player has less than the quantity
	if (m_itemsOwned[itemName].quantity < quantity) {
		return false;
	}

	// otherwise, they have it!
	return true;
}

//***************************************************************
// These function are relative to the Selector component of the menu, and
// It's subclasses

Inventory::Selector* Inventory::Selector::create(Node* background, Inventory::Previewer* previewer) {
	Selector *pRet = new(std::nothrow)Selector();
	if (pRet && pRet->init(background, previewer))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool Inventory::Selector::init(Node* background, Inventory::Previewer* previewer) {
	m_background = background;
	m_previewer = previewer;

#define PREVIEWER_SCALE  4
	if (!Node::init()) {
		return false;
	}
	setContentSize(background->getContentSize() / 2);
	int itemIndex = 0;
	for (std::unordered_map<std::string, InventoryItem>::iterator currentItem = m_itemInformation.begin(); currentItem != m_itemInformation.end(); currentItem++) {
		std::string itemName = currentItem->first;
		InventoryItem itemInformation = currentItem->second;
		if (requirement(itemName)) {
			MenuItem* craftingItemTag = MenuItem::create();

			craftingItemTag->setContentSize(Size(2 * ITEM_SIZE, ITEM_SIZE));
			craftingItemTag->setScale(PREVIEWER_SCALE);
			craftingItemTag->setAnchorPoint(Vec2(0,0));
			
			std::string itemSpriteName = itemInformation.sprite.empty() ? itemName: itemInformation.sprite;

			Node* itemPicture = SpriteLoader::loadSprite(itemSpriteName, SPRITE_ITEM);

			if (itemPicture == nullptr) {
				// Sprite loading failed, skip
				continue;
			}

			itemPicture->setAnchorPoint(Vec2(0, 1));
			craftingItemTag->addChild(itemPicture);

			Node* userObject = getUserObject(itemName);
			userObject->setPosition(Vec2(ITEM_SIZE,0));
			craftingItemTag->addChild(userObject);

			craftingItemTag->setPosition(Vec2((int)(itemIndex % COLOMN_COUNT) * craftingItemTag->getContentSize().width * PREVIEWER_SCALE, getContentSize().height - (int)(itemIndex / COLOMN_COUNT)* ITEM_SIZE * PREVIEWER_SCALE));
			addChild(craftingItemTag);


			EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
			listener->onTouchBegan = [itemName, craftingItemTag, previewer](cocos2d::Touch* touch, cocos2d::Event* event) {
				Vec2 touchLocation = touch->getLocation();

				Size currentSize = craftingItemTag->getContentSize() * PREVIEWER_SCALE;
				Vec2 worldPosition = craftingItemTag->getParent()->convertToWorldSpace(craftingItemTag->getPosition() - Vec2(0, ITEM_SIZE * PREVIEWER_SCALE));
				Node* currentNode = craftingItemTag;
				while ((currentNode = currentNode->getParent()) != nullptr){ // this works, gets the right size... Basically times the size by all the parent's scales
					currentSize = currentSize*currentNode->getScale();
				} 
				
				Rect worldRect = Rect(worldPosition, currentSize);
				if (worldRect.containsPoint(touchLocation)) {
					previewer->setSelectedItem(itemName);
					return true;
				}
				return false;
			};

			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, craftingItemTag);
			itemIndex++;
		}
	}
	return true;
}

void Inventory::Selector::refreshItems() {
	if (getParent() == nullptr) {
		cleanup();
		create(m_background, m_previewer);
	}
	else {
		Node* parent = getParent();
		removeAllChildren();
		init(parent, m_previewer);
	}
}

Inventory::CraftingSelector* Inventory::CraftingSelector::create(Node* background) {
	CraftingSelector* pRet = new(std::nothrow)CraftingSelector();
	if (pRet && pRet->init(background))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool Inventory::CraftingSelector::init(Node* background) {
	if (!Selector::init(background, m_craftingItemPreviewer)) { // Initialiazing the super
		return false;
	}
	return true;
};

Node* Inventory::CraftingSelector::getUserObject(std::string itemName) {
	Node* re = Node::create();
	if (m_itemInformation.find(itemName) == m_itemInformation.end()) return re;
	InventoryItem itemInformation =m_itemInformation[itemName];
	int i = 0;
	auto recipe = getFirstCraftableRecipe(itemInformation.recipies);
	for (Recipe::iterator currentIngredient = recipe->begin(); currentIngredient != recipe->end(); currentIngredient++) {
		std::string ingredientResource = currentIngredient->first;
		int ingredientQuantity = currentIngredient->second;
			
		Node* ingredient = Node::create();
		ingredient->setAnchorPoint(Vec2(0, 1));
		
		std::string spriteName = itemInformation.sprite.empty() ? ingredientResource : itemInformation.sprite;

		Node* resourceSprite = SpriteLoader::loadSprite(ingredientResource, SPRITE_ITEM);
		resourceSprite->setScale(0.5);
		resourceSprite->setAnchorPoint(Vec2(0, 1));

		ingredient->addChild(resourceSprite);
		XBMPLabel* quantityLabel = XBMPLabel::create(StringOps::to_string(ingredientQuantity),
			"Pixelfont",
			11,
			XBMPLabel::LEFT);
		quantityLabel->setAnchorPoint(Vec2(0, 1));
		quantityLabel->setPosition(Vec2(SMALL_RESOURCE_SIZE, 0));
		ingredient->addChild(quantityLabel);
		ingredient->setPosition(Vec2((int)(i / 2)*(SMALL_RESOURCE_SIZE + 10), SMALL_RESOURCE_SIZE * -(i % 2)));
		re->addChild(ingredient);
		i++;
	}
	// so that only 1 recipe is shown
	return re;
}

bool Inventory::CraftingSelector::requirement(std::string itemName) {
	return canCraftItem(itemName);
}

Inventory::InventorySelector* Inventory::InventorySelector::create(Node* background) {
	InventorySelector* pRet = new(std::nothrow)InventorySelector();
	if (pRet && pRet->init(background))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool Inventory::InventorySelector::init(Node* background) {
	if (!Selector::init(background,m_inventoryPreviewer)) { // Initialiazing the super
		return false;
	}
	return true;
};

Node* Inventory::InventorySelector::getUserObject(std::string itemName) {
	Node* re = Node::create();
	XBMPLabel* quantity = XBMPLabel::create(StringOps::to_string(m_itemsOwned[itemName].quantity),
		"Pixelfont", 11, XBMPLabel::LEFT);
	re->addChild(quantity);
	return re;
};

bool Inventory::InventorySelector::requirement(std::string itemName) {
	if (m_itemsOwned.find(itemName) == m_itemsOwned.end()) return false;
	if (m_itemsOwned[itemName].quantity > 0) {
		return true;
	}
	return false;
}

//***************************************************************
// These functions are relative to the Previewer component of the menu,
// and it's subclasses

Inventory::Previewer* Inventory::Previewer::create(Node* background, std::string defaultItem) {
	Previewer *pRet = new(std::nothrow)Previewer();
	if (pRet && pRet->init(background, defaultItem))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
};

bool Inventory::Previewer::init(Node* background, std::string defaultItem) {
	m_background = background;

	// No display if no default item
	if (defaultItem.empty()) return true;

	// If there is no such item, no display
	if (m_itemInformation.find(defaultItem) == m_itemInformation.end()) return true;

	// if false, return true (not kidding, check Node::init def)
	if (!Node::init()) {
		return false;
	}
#define PREVIEW_ITEM_SCALE 6
	setContentSize(background->getContentSize() / 2);
	setAnchorPoint(Vec2(0, 0));

	InventoryItem itemInformation = m_itemInformation[defaultItem];

	std::string spriteName = itemInformation.sprite.empty() ? defaultItem : itemInformation.sprite;

	Node* itemImage = SpriteLoader::loadSprite(spriteName, SPRITE_ITEM);
	itemImage->setAnchorPoint(Vec2(0, 1));
	itemImage->setPosition(Vec2(0, background->getContentSize().height/2));
	itemImage->setScale(PREVIEW_ITEM_SCALE);
	addChild(itemImage);


	XBMPLabel* nameLabel = XBMPLabel::create(defaultItem,
		"Pixelfont",
		11,
		XBMPLabel::LEFT);

	nameLabel->setPosition(Vec2(ITEM_SIZE * PREVIEW_ITEM_SCALE, background->getContentSize().height / 2));
	nameLabel->setScale(4);
	nameLabel->setAnchorPoint(Vec2(0, 1));
	addChild(nameLabel);

	XBMPLabel* descriptionLabel = XBMPLabel::create(itemInformation.description, 
		"Pixelfont",
		6,
		XBMPLabel::LEFT);
	descriptionLabel->setPosition(Vec2(0 , (background->getContentSize().height / 2) - ITEM_SIZE*PREVIEW_ITEM_SCALE));
	descriptionLabel->setScale(4);
	descriptionLabel->setAnchorPoint(Vec2(0, 1));
	addChild(descriptionLabel);
	return true;

}

void Inventory::Previewer::setSelectedItem(std::string itemName) {
	m_selectedItem = itemName;
	if (getParent() == nullptr) {
		cleanup();
		create(m_background, itemName);
	}
	else {
		Node* parent = getParent();
		removeAllChildren();
		init(parent,itemName);
	}
}

std::string Inventory::Previewer::getSelectedItem() {
	return m_selectedItem;
}

Inventory::CraftingPreviewer* Inventory::CraftingPreviewer::create(Node* background, std::string defaultItem) {
	CraftingPreviewer *pRet = new(std::nothrow)CraftingPreviewer();
	if (pRet && pRet->init(background, defaultItem))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool Inventory::CraftingPreviewer::init(Node* background, std::string defaultItem) {


	// initialize the Previewer parent
	if (!Previewer::init(background, defaultItem)) {
		return false;
	}

	if (defaultItem.empty()) return true;

	Node* ingredients = Node::create();
	ingredients->setAnchorPoint(Vec2(0, 1));
	int i = 0;
	if (m_itemInformation.find(defaultItem) == m_itemInformation.end()) return true;
	InventoryItem itemInformation = m_itemInformation[defaultItem];
	auto recipe = getFirstCraftableRecipe(itemInformation.recipies);
	for (Recipe::iterator currentIngredient = recipe->begin();currentIngredient != recipe->end(); currentIngredient++) {
		std::string ingredientResource = currentIngredient->first;
		int ingredientQuantity = currentIngredient->second;

		Node* ingredient = Node::create();
		ingredient->setAnchorPoint(Vec2(0, 1));

		std::string spriteName = itemInformation.sprite.empty() ? ingredientResource : itemInformation.sprite;

		Node* resourceSprite = SpriteLoader::loadSprite(spriteName, SPRITE_ITEM);
		resourceSprite->setScale(0.5);
		resourceSprite->setAnchorPoint(Vec2(0, 1));

		ingredient->addChild(resourceSprite);
		XBMPLabel* quantityLabel = XBMPLabel::create(StringOps::to_string(ingredientQuantity),
			"Pixelfont",
			11,
			XBMPLabel::LEFT);
		quantityLabel->setAnchorPoint(Vec2(0, 1));
		quantityLabel->setPosition(Vec2(SMALL_RESOURCE_SIZE, 0));
		ingredient->addChild(quantityLabel);
		ingredient->setPosition(Vec2((int)(i / 2)*(SMALL_RESOURCE_SIZE + 10), SMALL_RESOURCE_SIZE * -(i % 2)));
		ingredients->addChild(ingredient);
		i++;
		}
	ingredients->setPosition(Vec2(ITEM_SIZE * PREVIEW_ITEM_SCALE, (background->getContentSize().height / 2) - 11 * 4));
	ingredients->setScale(4);
	addChild(ingredients);


	Sprite* craftButton = Sprite::create(CRAFTING_IMAGE);
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	craftButton->setScale(4);
	craftButton->setAnchorPoint(Vec2(0,0));
	craftButton->setPosition(Vec2(background->getContentSize().width / 2 - craftButton->getContentSize().width * 4, 0));
	addChild(craftButton);
	listener->onTouchBegan = CC_CALLBACK_2(Inventory::CraftingPreviewer::callCraftCallback, this);
	m_craftButton = craftButton;

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, craftButton);
	return true;
}

void Inventory::CraftingPreviewer::setCraftCallback(std::function<void()> callback) {
	m_craftCallBack = callback;
}

bool Inventory::CraftingPreviewer::callCraftCallback(cocos2d::Touch* touch, cocos2d::Event* event) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 touchLocation = touch->getLocation();

		Size currentSize = m_craftButton->getContentSize() * m_craftButton->getScale();
		Vec2 worldPosition = m_craftButton->getParent()->convertToWorldSpace(m_craftButton->getPosition());
		Node* currentNode = m_craftButton;
		while ((currentNode = currentNode->getParent()) != nullptr) { // this works, gets the right size... Basically times the size by all the parent's scales
			currentSize = currentSize*currentNode->getScale();
		}
		
		std::string itemName = getSelectedItem();

		Rect worldRect = Rect(worldPosition, currentSize);
		if (worldRect.containsPoint(touchLocation)) {
			auto recipe = getFirstCraftableRecipe(m_itemInformation[itemName].recipies);
			for (std::unordered_map<std::string, int>::iterator currentIngredient = recipe->begin(); currentIngredient != recipe->end(); currentIngredient++) {
				takeItem(currentIngredient->first, currentIngredient->second);
			}
			giveItem(itemName, 1);
			return true;
		}
		return false;
}

void Inventory::takeItem(std::string itemName, int quantity) {
	giveItem(itemName, -quantity);
}

void Inventory::reconstruct() {
	m_inventorySelector->refreshItems();
	m_craftingList->refreshItems();
	if (!m_craftingList->requirement(m_craftingItemPreviewer->getSelectedItem())) { // If you cannot craft the item, do not preview it
		m_craftingItemPreviewer->setSelectedItem("");
	}
	if (!m_inventorySelector->requirement(m_inventoryPreviewer->getSelectedItem())) { // If you don't own it anymore, don't view it
		m_inventoryPreviewer->setSelectedItem("");
	}
}

int Inventory::getQuantityOf(std::string itemName) {
	if (m_itemsOwned.find(itemName) == m_itemsOwned.end()) return 0;
	return m_itemsOwned[itemName].quantity;
}

std::unordered_map<std::string, InventoryItem> Inventory::getItems() {
	return m_itemInformation;
}

Inventory::InventoryPreviewer* Inventory::InventoryPreviewer::create(Node* background, std::string defaultItem) {
	InventoryPreviewer* re = new InventoryPreviewer();
	if (re && re->init(background, defaultItem)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool Inventory::InventoryPreviewer::init(Node* background, std::string defaultItem) {
	if (!Previewer::init(background, defaultItem)) return false;

	std::list<ItemUse> uses = m_itemInformation[defaultItem].uses;
	int i = 0;

	// For every use
	for (auto use : uses) {
		if (i > 2) break; // Only display first 3 interactions
		LuaItemUses conditionalInterpreter;
		conditionalInterpreter.init(defaultItem);
		if (!(use.conditional.empty() || conditionalInterpreter.fulfills(std::list<std::string>(), use.conditional))) continue;
		XBMPLabel* useLabel = XBMPLabel::create(use.name, "Pixelfont", 44, XBMPLabel::LEFT);
		useLabel->setPosition((background->getContentSize().height/4) + i/4, (background->getContentSize().height/2) - 44*(i % 4) -44);
		useLabel->setAnchorPoint(Vec2(0, 1));

		useLabel->setCallback([use, defaultItem]() {
			LuaItemUses interpreter;
			interpreter.init(defaultItem);
			interpreter.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_ITEMS), use.command); // empty string for no script files
		});

		addChild(useLabel);
		i++;
	}
	return true;
}