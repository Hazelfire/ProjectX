#pragma once
#include <string>
#include <vector>
#include "Parser/Items.h"
#include <map>
#include "cocos2d.h"
#include <unordered_map>
#include "Arena.h"
#include "XBMPLabel.h"
USING_NS_CC;
struct ItemProperties {
	int quantity;
};

class Inventory {
public:
	// Give the player the item specified
	static void giveItem(std::string itemName, int quantity);
	static void takeItem(std::string itemName, int quantity);
	static int getQuantityOf(std::string itemname);
	static void openInventory();
	static void closeInventory();
	static bool touchEvent(Touch* touch, Event* touchEvent);
	static bool canCraftItem(std::string itemName);
	static Node* createInventoryMenu();
	static bool hasItem(std::string, int quantity);
	static std::unordered_map<std::string, InventoryItem> getItems(); //Returns items owned by the player
private:
	static std::map<std::string,ItemProperties> m_itemsOwned;
	static std::unordered_map<std::string, InventoryItem> m_itemInformation;
	static Node* m_inventoryMenu;	
	static void reconstruct();

	static std::list<Recipe>::iterator getFirstCraftableRecipe(std::list<Recipe> &recepies);

	static bool satisfiesRecipe(Recipe recipe);
	class Previewer : public Node {
	public:
		static Previewer* create(Node* background, std::string defaultItem);
		virtual bool init(Node* background, std::string defaultItem);
		virtual void setSelectedItem(std::string itemName);
		virtual std::string getSelectedItem();
	protected:
		Node* m_background;
		std::string m_selectedItem;
	};

	class CraftingPreviewer : public Previewer {
	public:
		static CraftingPreviewer* create(Node* background, std::string defaultItem);
		virtual bool init(Node* background, std::string defaultItem);

		
		void setCraftCallback(std::function<void()>);
		bool callCraftCallback(cocos2d::Touch* touch, cocos2d::Event* event);
	private:
		Sprite* m_craftButton;
		std::function<void()> m_craftCallBack;
	};

	class InventoryPreviewer : public Previewer {
	public:
		static InventoryPreviewer* create(Node* background, std::string defaultItem);
		virtual bool init(Node* background, std::string defaultItem);

	};

	class Selector: public Node {
	public:
		static Selector* create(Node* backgound, Previewer* previewer);
		virtual bool init(Node* background, Previewer* previewer);
		virtual Node* getUserObject(std::string itemName) { return nullptr; };
		virtual void refreshItems();
		virtual bool requirement(std::string itemName) { return false; };
	protected:
		Node* m_background;
		Previewer* m_previewer;

	};

	class CraftingSelector : public Selector {
	public:
		static CraftingSelector* create(Node* background);
		virtual bool init(Node* background);
		virtual Node* getUserObject(std::string itemName);
		virtual bool requirement(std::string itemName);
	protected:
	};

	class InventorySelector : public Selector {
	public:
		static InventorySelector* create(Node* background);
		virtual bool init(Node* background);
		virtual Node* getUserObject(std::string itemName);
		virtual bool requirement(std::string itemName);
	protected:
	};

	static InventoryPreviewer* m_inventoryPreviewer;
	static InventorySelector* m_inventorySelector;
	static CraftingPreviewer* m_craftingItemPreviewer;
	static CraftingSelector* m_craftingList;
};