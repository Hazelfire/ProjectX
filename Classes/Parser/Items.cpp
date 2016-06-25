#include "Items.h"
#include <cstring>
#include "StringOperations.h"
#include <sstream>
#include "XMLCommon.h"
#include "rapidxml.hpp"
using std::strcpy;
using std::strcmp;
struct ItemsParser::Impl {
	static InventoryItem parseItem(rapidxml::xml_node<>*);
};

ItemsParser::Impl ItemsParser::impl;

using namespace rapidxml;
AllInventoryItems ItemsParser::parse(std::string source) {
	char* newText = new char[source.size() + 1];
	strcpy(newText, source.c_str());

	xml_document<> doc;
	strcpy(newText, source.c_str());
	doc.parse<0>(newText);

	AllInventoryItems re;

	for (xml_node<>* childNode = doc.first_node(); childNode; childNode =childNode->next_sibling()) {
		if (strcmp(childNode->name(), "item") == 0) {
			re.items[childNode->first_attribute()->value()] = impl.parseItem(childNode);
		}
	}
	delete[] newText;
	return re;
}

InventoryItem ItemsParser::Impl::parseItem(rapidxml::xml_node<>* itemNode) {
	InventoryItem re;
	for (xml_node<>* childNode = itemNode->first_node(); childNode; childNode = childNode->next_sibling()) {
		if (strcmp(childNode->name(), "ingredients") == 0) {
			std::stringstream ingredients(std::string(childNode->value()));
			std::string currentIngredient;
			Recipe currentRecipe;
			while (std::getline(ingredients, currentIngredient, ';')) {
				if (StringOps::trim(currentIngredient).empty()) continue;
				std::string ingredientToken= StringOps::trim(currentIngredient.substr(0, currentIngredient.find(':')));
				int quantityToken = std::atoi(StringOps::trim(currentIngredient.substr(currentIngredient.find(':') +1, currentIngredient.length())).c_str());
				currentRecipe[ingredientToken] = quantityToken;
			}
			re.recipies.push_back(currentRecipe);
		}
		else if (strcmp(childNode->name(), "description") == 0) {
			re.description = StringOps::trim(std::string(childNode->value()));
		}
		else if (strcmp(childNode->name(), "tags") == 0) {
			std::stringstream tags(std::string(childNode->value()));
			std::string currentTag;
			while (std::getline(tags, currentTag, ';')) {
				if (StringOps::trim(currentTag).empty()) continue;
				re.tags.push_back(StringOps::trim(currentTag));
			}
		}
	}
	return re;
}
