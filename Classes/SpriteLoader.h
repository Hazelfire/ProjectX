#include "cocos2d.h"
#include "Parser/SpriteParser.h"
#include "AnimatedSprite.h"
#include "SpriteTypes.h"

class SpriteLoader {
public:

	// Loads a sprite based on it's name, seems trivial but it also loads animated sprites as well
	static cocos2d::Node* loadSprite(std::string spriteName, SpriteType spriteType);

	static cocos2d::Node* loadSprite(std::string spriteName);
private:
	static cocos2d::Node* searchSpritesFor(SpriteParser::SpriteSheet*,int spriteSheetSize, std::string spriteName);

	// Information required to construct a static sprite
	struct ConstructSpriteInformation {
		std::string source;
		int spriteWidth;
		int spriteHeight;
		int x;
		int y;
	};

	static cocos2d::Node* constructSprite(ConstructSpriteInformation);
	static cocos2d::Node* constructAnimate(AnimatedSprite::ConstructAnimateInformation);
};