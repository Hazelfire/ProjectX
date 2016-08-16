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
	static SpriteSheet parseSpritesheet(xml_node<>*, SpriteType);
};

SpriteParser::Impl SpriteParser::impl;

SpriteParser::SpriteSheetSet SpriteParser::parse(std::string source, SpriteType type) {

	// Convert it into char* (non const because xml_document does not accept const)
	char* newText = new char[source.size() +1];
	strcpy(newText, source.c_str());

	// Parse
	xml_document<> doc;
	strcpy(newText,source.c_str());
	doc.parse<0>(newText);

	// Initialize return value
	SpriteSheetSet re;
	
	int spriteCount = 0;

	// count nodes
	for (xml_node<>* spriteSheetNode = doc.first_node("spritesheet"); spriteSheetNode;spriteSheetNode = spriteSheetNode->next_sibling("spritesheet")) {
		spriteCount++;
	}
	
	// setting sizes
	re.spriteCount = spriteCount;

	// initializing arrays
	re.sprites = new SpriteSheet[re.spriteCount];

	int spriteIndex = 0;

	// filling spritesheets
	for (xml_node<>* spriteSheetNode = doc.first_node("spritesheet"); spriteSheetNode;spriteSheetNode = spriteSheetNode->next_sibling("spritesheet")) {
		re.sprites[spriteIndex] = impl.parseSpritesheet(spriteSheetNode, type);
		if(type == SPRITE_TILES){
			int tileWidth = std::atoi(spriteSheetNode->first_attribute("tileWidth")->value());
			int tileHieght = std::atoi(spriteSheetNode->first_attribute("tileHeight")->value());

			re.sprites[spriteIndex].spriteWidth = MAPTILE_WIDTH * tileWidth;
			re.sprites[spriteIndex].spriteHeight = MAPTILE_HEIGHT * tileHieght;
		}

		else if (type == SPRITE_ITEMS) {
			re.sprites[spriteIndex].spriteWidth = 32;
			re.sprites[spriteIndex].spriteHeight = 32;
		}
		spriteIndex++;
	}

	delete[] newText;
	return re;
}

SpriteParser::SpriteSheet SpriteParser::Impl::parseSpritesheet(xml_node<>* spriteSheetNode, SpriteParser::SpriteType type) {
	SpriteSheet re;

	// For every attribute
	for (xml_attribute<>* spriteSheetAttr = spriteSheetNode->first_attribute(); spriteSheetAttr; spriteSheetAttr = spriteSheetAttr->next_attribute()) {
		if (strcmp(spriteSheetAttr->name(), "source") == 0) {
			std::string path = SPRITES_FOLDER;

			switch (type) {
			case SPRITE_ITEMS:
				path += "items/";
				break;
			case SPRITE_CREATURES:
				path += "creatures/";
				break;
			case SPRITE_PLAYER:
				path += "players/";
				break;
			case SPRITE_TILES:
				path += "tiles/";
				break;
			}

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
