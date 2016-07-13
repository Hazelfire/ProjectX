#include "Arena.h"
#include "MapGen/MapGenerator.h"
#include "Inventory.h"
#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include "Lua/LuaGame.h"
#include "ScriptLoader.h"
#include "Debug.h"
#include "Parser/StringOperations.h"
#include "Save.h"
#include "MyUI/LuaTerminal.h"

int WIDTH;
int HEIGHT;

//class information needs to be declared in c++ (if static)
XTileMap* Arena::m_arenaMap;
Scene* Arena::m_arenaScene;
Layer* Arena::m_UILayer;
Node* Arena::m_inventoryMenu;
int Arena::m_state = 0;
Layer* Arena::m_camera;
bool Arena::m_termOpen;

void Arena::createMulti(std::string map, int seed) {

	Debugger::log("Creating multiplayer game with map: " + map + " and seed: " + StringOps::to_string(seed), DEBUG_GENERIC);

	m_arenaScene = Scene::create();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//The layer arangement that I used seems a bit stange, all you need to know is that the camera is the youngest layer. (in a children sort of way)
	Layer* viewport = Layer::create();
	viewport->setScale(PIXELATION);
	viewport->setName("viewport");
	m_camera = Layer::create();
	viewport->addChild(m_camera);
	m_arenaScene->addChild(viewport);


	m_arenaMap = XTileMap::create(map, seed);
	m_camera->addChild(m_arenaMap);

	addUI();
}

Layer* Arena::getCameraInstance() {
	return m_camera;
}

void Arena::create(GameInformation gameInfo){


	Debugger::log("Creating singleplayer game with map: " + gameInfo.mapName, DEBUG_GENERIC);

	m_arenaScene = Scene::create();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//The layer arangement that I used seems a bit stange, all you need to know is that the camera is the youngest layer. (in a children sort of way)
	Layer* viewport = Layer::create();
	viewport->setScale(PIXELATION);
	viewport->setName("viewport");
	m_camera = Layer::create();
	viewport->addChild(m_camera);
	m_arenaScene->addChild(viewport);


	m_arenaMap = XTileMap::create(gameInfo.mapName);
	m_camera->addChild(m_arenaMap);
	Player* player = Player::create(gameInfo.playerInfo.classIndex, gameInfo.playerInfo.playerName, 0);
	placePlayer(player);
	addUI();

	LuaGame startupScript;
	startupScript.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_STARTUP));
}

void Arena::addKeyControls() {
	EventListenerKeyboard* devControlsListener = EventListenerKeyboard::create();
	devControlsListener->onKeyPressed = Arena::keyPressed;

	// quite a few things take precedent over
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(devControlsListener, getArenaScene());
}

void Arena::keyPressed(EventKeyboard::KeyCode code, Event*) {
	if (Save::read(&SaveInformation::devMode)) {
		switch (code) {
		case EventKeyboard::KeyCode::KEY_C:
			toggleTerminal();
			break;
		default:
			break;
		}
	}
	if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
		
	}
}

void Arena::toggleTerminal(){
	
}

Scene * Arena::getArenaScene()
{
	return m_arenaScene;
}

Player* Arena::getPlayerInstance() {
	return Player::getInstance();
}

Vec2i Arena::placePlayer(Player * player) 
{
	Vec2i position;
	XTileMap* map = getMapInstance();
	do {
		map->cullChunkBlock(map->getChunkCoordinates(position)); // if the tile is not rendered, it will not cull. these renderings make sure that the tile is movable
		position = Vec2i(RandomHelper::random_int(0,map->getMapSize().x), RandomHelper::random_int(0, map->getMapSize().y));//Vec2i(RandomHelper::random_int(0, (int)map->getMapSize().x - 1), RandomHelper::random_int(0, (int)map->getMapSize().y - 1));
		map->renderChunkBlock(map->getChunkCoordinates(position));
	} while (!map->isMovable(position));
	map->placeNode(position, player);

	return position;
}

XTileMap * Arena::getMapInstance()
{
	return m_arenaMap;
}


void Arena::addUI() {
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	m_UILayer = Layer::create();
	m_UILayer->setName("UI");
	m_arenaScene->addChild(m_UILayer, 1);
	m_UILayer->setPosition(origin);
	Sprite* infoBar = Sprite::create(INFO_BAR_IMAGE);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	infoBar->setAnchorPoint(Vec2(0, 1));
	infoBar->setPosition(0, visibleSize.height);
	infoBar->setScale((float)visibleSize.width / infoBar->getContentSize().width);
	Size infoBarSize = infoBar->getContentSize();

	ui::Button* inventoryButton = ui::Button::create();
	inventoryButton->loadTextureNormal(INVENTORY_BUTTON);
	inventoryButton->setScale(((float)infoBarSize.height / inventoryButton->getContentSize().height)/2);
	inventoryButton->setAnchorPoint(Vec2(0, 0));
	inventoryButton->setPosition(Vec2(infoBarSize.width - inventoryButton->getContentSize().width * inventoryButton->getScale(), 0));

	inventoryButton->addTouchEventListener(CC_CALLBACK_2(Arena::inventoryOpened, this));

	Node* inventory = Inventory::createInventoryMenu();
	m_UILayer->addChild(inventory);

	infoBar->addChild(inventoryButton);
	// because the infobar needs to be higher than the inventory
	infoBar->setLocalZOrder(123);
	m_UILayer->addChild(infoBar);


	if (Save::read(&SaveInformation::devMode)) {
		LuaTerminal* term = LuaTerminal::create();

		m_UILayer->addChild(term);
	}
}

void Arena::inventoryOpened(Ref* sender, ui::Button::TouchEventType type) {
	if (type == ui::Button::TouchEventType::ENDED) {
		if (m_state == MENU_STATE) {
			Inventory::closeInventory();
		}
		else {
			Inventory::openInventory();
		}
	}
}

int Arena::getState() {
	return m_state;
}

void Arena::setState(int state) {
	m_state = state;
}

void Arena::displayText(std::string text) {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	Sprite* background = Sprite::create(INVENTORY_BACKGROUND_IMAGE);
	background->setScale(visibleSize.width / (background->getContentSize().width* 1.2));
	background->setPosition(visibleSize / 2);
	setState(MENU_STATE);
	m_UILayer->addChild(background);

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchEnded = [background](Touch* touch, Event* touchEvent) {
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 menuPosition = background->getPosition();
		Size menuSize = background->getContentSize() * background->getScale();
		Rect backgroundRect = Rect(menuPosition + (visibleSize / 2) - (menuSize / 2), menuSize);
		if (!backgroundRect.containsPoint(touch->getLocation())) {
			background->removeAllChildren();
			return true;
		}
		return false;
	};
}

Layer* Arena::getUILayer() {
	return m_UILayer;
}
