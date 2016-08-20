#include "cocos2d.h"
#include "Parser/SpriteParser.h"
#include "AnimatedSprite.h"
#include "SpriteTypes.h"

class SpriteLoader {
public:

	// Loads a sprite based on it's name, seems trivial but it also loads animated sprites as well
	static cocos2d::Node* loadSprite(std::string spriteName, SpriteType spriteType);

	static bool isAnimate(std::string spriteName, SpriteType spriteType);

	static cocos2d::Node* loadSprite(std::string spriteName);
private:
	static cocos2d::Node* searchSpritesFor(SpriteParser::SpriteSheet*,int spriteSheetSize, std::string spriteName, SpriteType type);
	static bool searchSpritesForType(SpriteParser::SpriteSheet*, int spritesheetSize, std::string spriteName, SpriteType type);

	// Information required to construct a static sprite
	struct ConstructSpriteInformation {
		std::string source;
		int spriteWidth;
		int spriteHeight;
		int x;
		int y;
		SpriteType type;
	};

	static cocos2d::Node* constructSprite(ConstructSpriteInformation);
	static cocos2d::Node* constructAnimate(AnimatedSprite::ConstructAnimateInformation);
};