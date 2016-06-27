#ifndef __ARENA_H__
#define __ARENA_H__
#include "cocos2d.h"
#include "Player.h"
#include "XTileMap.h"
#include "ui/UIButton.h"
#include "UIScenes/PlayerSelector.h"
USING_NS_CC;

#define PIXELATION 3
#define GAME_STATE 0
#define MENU_STATE 1

class Arena{
public:
	// creates the arena in multiplayer mode (with seed and no placed players)
	void createMulti(std::string map, int seed);
	void create(GameInformation);
	static Scene* getArenaScene();
	static XTileMap* getMapInstance();
	static Layer* getCameraInstance();

	// Deprecated, please use Player::getInstance() instead
	static Player* getPlayerInstance();

	void inventoryOpened(Ref* sender, ui::Button::TouchEventType type);
	static int getState();
	static void setState(int);

	// places player inside map, returns coordinates that the player spawned in
	static Vec2i placePlayer(Player * player);

	static Layer* getUILayer();

	static void displayText(std::string);

	void addKeyControls();

	static void keyPressed(EventKeyboard::KeyCode code, Event*);

private:
	static XTileMap* m_arenaMap;
	static Scene* m_arenaScene;
	static Layer* m_UILayer;
	static Layer* m_camera;
	static Node* m_inventoryMenu;
	static int m_state;
	void addUI();
};

#endif