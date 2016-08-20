#include "SpriteLoader.h"
#include "ScriptLoader.h"
#include "Debug.h"
#include "Packages/PackageManager.h"

cocos2d::Node* SpriteLoader::loadSprite(std::string spriteName) {
	cocos2d::Node* re = nullptr;
	re = loadSprite(spriteName, SPRITE_ITEM);
	if (re)
		return re;
	re = loadSprite(spriteName, SPRITE_PLAYER);
	if (re)
		return re;
	re = loadSprite(spriteName, SPRITE_CREATURE);
	if (re)
		return re;
	re = loadSprite(spriteName, SPRITE_TILE);
	if (re)
		return re;
	return nullptr;
}

cocos2d::Node* SpriteLoader::loadSprite(std::string spriteName, SpriteType type) {

	// Statically loads information
	SpriteParser::SpriteSheetSet spriteInformation;
	switch (type) {
	case SPRITE_CREATURE:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_CREATURES), SpriteParser::SPRITE_CREATURES);
		break;
	case SPRITE_ITEM:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_ITEMS), SpriteParser::SPRITE_ITEMS);
		break;
	case SPRITE_PLAYER:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_PLAYERS), SpriteParser::SPRITE_PLAYER);
		break;
	case SPRITE_TILE:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_TILES), SpriteParser::SPRITE_TILES);
		break;
	}

	cocos2d::Node* re = nullptr;
	re = searchSpritesFor(spriteInformation.sprites, spriteInformation.spriteCount, spriteName, type);

	return re;
}

bool SpriteLoader::isAnimate(std::string spriteName, SpriteType spriteType) {
	// Statically loads information
	SpriteParser::SpriteSheetSet spriteInformation;
	switch (spriteType) {
	case SPRITE_CREATURE:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_CREATURES), SpriteParser::SPRITE_CREATURES);
		break;
	case SPRITE_ITEM:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_ITEMS), SpriteParser::SPRITE_ITEMS);
		break;
	case SPRITE_PLAYER:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_PLAYERS), SpriteParser::SPRITE_PLAYER);
		break;
	case SPRITE_TILE:
		spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES_TILES), SpriteParser::SPRITE_TILES);
		break;
	}

	return searchSpritesForType(spriteInformation.sprites, spriteInformation.spriteCount, spriteName, spriteType);
}

cocos2d::Node* SpriteLoader::searchSpritesFor(SpriteParser::SpriteSheet* spritesheets,int size, std::string spriteName, SpriteType type) {

	// For every spritesheet
	for (int spritesheetIndex = 0; spritesheetIndex < size; spritesheetIndex++) {

		// For every non-animated Sprite
		for (int spriteIndex = 0; spriteIndex < spritesheets[spritesheetIndex].spritesSize; spriteIndex++) {
			
			// If we have found the sprite
			if (spritesheets[spritesheetIndex].sprites[spriteIndex].name == spriteName) {
				// Construct information needed to load the sprite
				ConstructSpriteInformation constructInfo;
				constructInfo.source = spritesheets[spritesheetIndex].source;
				constructInfo.spriteWidth = spritesheets[spritesheetIndex].spriteWidth;
				constructInfo.spriteHeight = spritesheets[spritesheetIndex].spriteHeight;
				constructInfo.x = spritesheets[spritesheetIndex].sprites[spriteIndex].x;
				constructInfo.y = spritesheets[spritesheetIndex].sprites[spriteIndex].y;
				constructInfo.type = type;

				return constructSprite(constructInfo);
			}
		}

		// We did not manage to find a static sprite, let's see if someone has animated It!

		// For every animated Sprite
		for (int animateIndex = 0; animateIndex < spritesheets[spritesheetIndex].animatedSize; animateIndex++) {

			// If we have found the sprite
			if (spritesheets[spritesheetIndex].animated[animateIndex].name == spriteName) {
				SpriteParser::AnimatedObject currentAnimate = spritesheets[spritesheetIndex].animated[animateIndex];
				// Construct Information needed to load animated sprite
				AnimatedSprite::ConstructAnimateInformation constructInfo;
				constructInfo.source = spritesheets[spritesheetIndex].source;
				constructInfo.spriteHeight = spritesheets[spritesheetIndex].spriteHeight;
				constructInfo.spriteWidth = spritesheets[spritesheetIndex].spriteWidth;
				constructInfo.startx = currentAnimate.startx;
				constructInfo.starty = currentAnimate.starty;
				constructInfo.width = currentAnimate.animateWidth;
				constructInfo.height = currentAnimate.animateHeight;
				constructInfo.speed = currentAnimate.speed;
				constructInfo.type = type;

				return constructAnimate(constructInfo);
			}
		}
	}
	Debugger::logError("Could not find sprite for " + spriteName, DEBUG_SPRITES);
	return nullptr;
}

bool SpriteLoader::searchSpritesForType(SpriteParser::SpriteSheet* spritesheets, int size, std::string spriteName, SpriteType spriteType) {
	// For every spritesheet
	for (int spritesheetIndex = 0; spritesheetIndex < size; spritesheetIndex++) {

		// For every non-animated Sprite
		for (int spriteIndex = 0; spriteIndex < spritesheets[spritesheetIndex].spritesSize; spriteIndex++) {

			// If we have found the sprite
			if (spritesheets[spritesheetIndex].sprites[spriteIndex].name == spriteName) {
				// Construct information needed to load the sprite
				return false;
			}
		}

		// We did not manage to find a static sprite, let's see if someone has animated It!

		// For every animated Sprite
		for (int animateIndex = 0; animateIndex < spritesheets[spritesheetIndex].animatedSize; animateIndex++) {

			// If we have found the sprite
			if (spritesheets[spritesheetIndex].animated[animateIndex].name == spriteName) {
				return true;
			}
		}
	}
	return false;
}

cocos2d::Node* SpriteLoader::constructSprite(ConstructSpriteInformation spriteInformation) {
	cocos2d::Node* re = Node::create();

	std::string spritePath = PackageManager::getInstance()->getSprite(spriteInformation.source, spriteInformation.type);

	if (!spritePath.empty()) {
		// Construct Sprite
		cocos2d::Sprite* sprite = cocos2d::Sprite::create(spritePath, CC_RECT_PIXELS_TO_POINTS(cocos2d::Rect(spriteInformation.x * spriteInformation.spriteWidth, spriteInformation.spriteHeight * spriteInformation.y, spriteInformation.spriteWidth, spriteInformation.spriteHeight)));
		sprite->getTexture()->setAliasTexParameters();
		sprite->setAnchorPoint(Vec2(0, 0));

		// setting the content size
		re->setContentSize(sprite->getContentSize());

		// Add child
		re->addChild(sprite);
	}
	else {
		// Sprite is invalid, probably because there is no file with that name, let's check up
		// With that

		// If no such file
		Debugger::logError("No such file " + spriteInformation.source + " in spritesheets file", DEBUG_SPRITES);
	}
	return re;
}

cocos2d::Node* SpriteLoader::constructAnimate(AnimatedSprite::ConstructAnimateInformation constructInformation) {
	cocos2d::Node* re = Node::create();

	// Construct Animated Sprite
	AnimatedSprite* animatedSprite = AnimatedSprite::create(constructInformation);

	re->addChild(animatedSprite);

	re->setContentSize(animatedSprite->getContentSize());
	
	return re;
}