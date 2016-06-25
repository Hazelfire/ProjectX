#include "SpriteParser.h"
#include "../ResourceMacros.h"
#include "XMLCommon.h"
#include "SizeMacros.h"
#include "rapidxml.hpp"
#include <cstring>

using std::strcpy;
using std::strcmp;

using namespace rapidxml;

struct SpriteParser::Impl {
	static SpriteSheet parseSpritesheet(xml_node<>*);
};

SpriteParser::Impl SpriteParser::impl;

SpriteParser::SpriteSheetSet SpriteParser::parse(std::string source) {

	// Convert it into char* (non const because xml_document does not accept const)
	char* newText = new char[source.size() +1];
	strcpy(newText, source.c_str());

	// Parse
	xml_document<> doc;
	strcpy(newText,source.c_str());
	doc.parse<0>(newText);

	// Initialize return value
	SpriteSheetSet re;
	
	int tilesCount = 0;
	int creaturesCount = 0;
	int itemsCount = 0;
	int playersCount = 0;

	// count nodes
	for (xml_node<>* sectionNode = doc.first_node(); sectionNode; sectionNode = sectionNode->next_sibling()) {
		for (xml_node<>* spriteSheetNode = sectionNode->first_node("spritesheet"); spriteSheetNode;spriteSheetNode = spriteSheetNode->next_sibling("spritesheet")) {
			if (strcmp(sectionNode->name(), "tiles") == 0)
				tilesCount++;

			else if (strcmp(sectionNode->name(), "creatures") == 0) 
				creaturesCount++;
			

			else if (strcmp(sectionNode->name(), "items") == 0) 
				itemsCount++;
			

			else if (strcmp(sectionNode->name(), "players") == 0) 
				playersCount++;
			
		}
	}
	
	// setting sizes
	re.tilesSize = tilesCount;
	re.creaturesSize = creaturesCount;
	re.itemsSize = itemsCount;
	re.playersSize = playersCount;

	// initializing arrays
	re.items = new SpriteSheet[re.itemsSize];
	re.creatures = new SpriteSheet[re.creaturesSize];
	re.tiles = new SpriteSheet[re.tilesSize];
	re.players = new SpriteSheet[re.playersSize];

	int tileIndex = 0;
	int creatureIndex = 0;
	int itemIndex = 0;
	int playerIndex = 0;

	// filling spritesheets
	for (xml_node<>* sectionNode = doc.first_node(); sectionNode; sectionNode = sectionNode->next_sibling()) {
		for (xml_node<>* spriteSheetNode = sectionNode->first_node("spritesheet"); spriteSheetNode;spriteSheetNode = spriteSheetNode->next_sibling("spritesheet")) {
			if (strcmp(sectionNode->name(), "tiles") == 0) {
				re.tiles[tileIndex] = impl.parseSpritesheet(spriteSheetNode);
				int tileWidth = std::atoi(spriteSheetNode->first_attribute("tileWidth")->value());
				int tileHieght = std::atoi(spriteSheetNode->first_attribute("tileHeight")->value());

				re.tiles[tileIndex].spriteWidth = MAPTILE_WIDTH * tileWidth;
				re.tiles[tileIndex].spriteHeight = MAPTILE_HEIGHT * tileHieght;
				tileIndex++;
			}

			else if (strcmp(sectionNode->name(), "creatures") == 0) {
				re.creatures[creatureIndex] = impl.parseSpritesheet(spriteSheetNode);
				creatureIndex++;
			}

			else if (strcmp(sectionNode->name(), "items") == 0) {
				re.items[itemIndex] = impl.parseSpritesheet(spriteSheetNode);
				re.items[itemIndex].spriteWidth = 32;
				re.items[itemIndex].spriteHeight = 32;
				itemIndex++;
			}

			else if (strcmp(sectionNode->name(), "players") == 0) {
				re.players[playerIndex] = impl.parseSpritesheet(spriteSheetNode);
				playerIndex++;
			}
		}
	}

	delete[] newText;
	return re;
}

SpriteParser::SpriteSheet SpriteParser::Impl::parseSpritesheet(xml_node<>* spriteSheetNode) {
	SpriteSheet re;

	// For every attribute
	for (xml_attribute<>* spriteSheetAttr = spriteSheetNode->first_attribute(); spriteSheetAttr; spriteSheetAttr = spriteSheetAttr->next_attribute()) {
		if (strcmp(spriteSheetAttr->name(), "source") == 0) {
			std::string path = SPRITES_FOLDER;
			path += spriteSheetNode->parent()->name();
			path += "/";
			re.source = path + spriteSheetAttr->value();
		}
		else if (strcmp(spriteSheetAttr->name(), "width") == 0) {
			re.spriteWidth = std::atoi(spriteSheetAttr->value());
		}
		else if (strcmp(spriteSheetAttr->name(), "height") == 0) {
			re.spriteHeight = std::atoi(spriteSheetAttr->value());
		}
	}


	// Parsing static sprites

	// counting sprites
	int spritesCount = 0;
	for (xml_node<>* spriteNode = spriteSheetNode->first_node("sprite"); spriteNode; spriteNode = spriteNode->next_sibling("sprite")) {
		spritesCount++;
	}

	re.spritesSize = spritesCount;
	re.sprites = new Sprite[re.spritesSize];

	// filling sprites
	int spriteIndex = 0;
	for (xml_node<>* spriteNode = spriteSheetNode->first_node("sprite"); spriteNode; spriteNode = spriteNode->next_sibling("sprite")) {
		Sprite currentSprite;
	
		currentSprite.name = spriteNode->first_attribute("name")->value();
		currentSprite.x = std::atoi(spriteNode->first_attribute("x")->value());
		currentSprite.y = std::atoi(spriteNode->first_attribute("y")->value());

		re.sprites[spriteIndex] = currentSprite;
		spriteIndex++;
	}

	// counting animates
	int animatedCount = 0;
	for (xml_node<>* animatedNode = spriteSheetNode->first_node("animate"); animatedNode; animatedNode = animatedNode->next_sibling("animate")) {
		animatedCount++;
	}

	re.animatedSize = animatedCount;
	re.animated = new AnimatedObject[re.animatedSize];

	// placing animates
	int animateIndex = 0;
	for (xml_node<>* animatedNode = spriteSheetNode->first_node("animate"); animatedNode; animatedNode = animatedNode->next_sibling("animate")) {
		AnimatedObject currentAnimate;
		
		currentAnimate.name = animatedNode->first_attribute("name")->value();
		currentAnimate.startx = std::atoi(animatedNode->first_attribute("x")->value());
		currentAnimate.starty = std::atoi(animatedNode->first_attribute("y")->value());
		currentAnimate.animateWidth = std::atoi(animatedNode->first_attribute("width")->value());
		currentAnimate.animateHeight = std::atoi(animatedNode->first_attribute("height")->value());
		currentAnimate.speed = std::atof(animatedNode->first_attribute("speed")->value());

		re.animated[animateIndex] = currentAnimate;
		animateIndex++;
	}
	
	return re;
}
