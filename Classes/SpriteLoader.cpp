#include "SpriteLoader.h"
#include "ScriptLoader.h"

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
	static const SpriteParser::SpriteSheetSet spriteInformation = SpriteParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_SPRITES));

	cocos2d::Node* re = nullptr;
	switch (type) {
	case SPRITE_ITEM:
		re = searchSpritesFor(spriteInformation.items, spriteInformation.itemsSize, spriteName);
		break;
	case SPRITE_CREATURE:
		re = searchSpritesFor(spriteInformation.creatures,spriteInformation.creaturesSize, spriteName);
		break;
	case SPRITE_TILE:
		re = searchSpritesFor(spriteInformation.tiles,spriteInformation.tilesSize, spriteName);
		break;
	case SPRITE_PLAYER:
		re = searchSpritesFor(spriteInformation.players, spriteInformation.playersSize, spriteName);
		break;
	}

	return re;
}

cocos2d::Node* SpriteLoader::searchSpritesFor(SpriteParser::SpriteSheet* spritesheets,int size, std::string spriteName) {

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

				return constructAnimate(constructInfo);
			}
		}
	}
	return nullptr;
}

cocos2d::Node* SpriteLoader::constructSprite(ConstructSpriteInformation spriteInformation) {
	cocos2d::Node* re = Node::create();

	// Construct Sprite
	cocos2d::Sprite* sprite = cocos2d::Sprite::create(spriteInformation.source,CC_RECT_PIXELS_TO_POINTS(cocos2d::Rect(spriteInformation.x * spriteInformation.spriteWidth, spriteInformation.spriteHeight * spriteInformation.y, spriteInformation.spriteWidth, spriteInformation.spriteHeight)));
	sprite->getTexture()->setAliasTexParameters();
	sprite->setAnchorPoint(Vec2(0, 0));

	// Add child
	re->addChild(sprite);

	// setting the content size
	re->setContentSize(sprite->getContentSize());
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