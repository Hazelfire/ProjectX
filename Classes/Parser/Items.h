#include <string>
#include <list>
#include <unordered_map>

typedef std::unordered_map<std::string, int> Recipe;

struct ItemUse {
	std::string name;
	std::string command;
	std::string conditional;
};

struct InventoryItem {
	std::string description;
	std::list<Recipe> recipies;
	std::list<std::string> tags;
	std::list<ItemUse> uses;
	std::string sprite;
};

struct AllInventoryItems {
	std::unordered_map<std::string,InventoryItem> items;
};
class ItemsParser {
public:
	//makes a struct for all the items available
	static AllInventoryItems parse(std::string source);
private:
	struct Impl;
	static Impl impl;
};
