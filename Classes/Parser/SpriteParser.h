#pragma once
#include <unordered_set>


class SpriteParser {
public:

	struct AnimatedObject {
		std::string name;
		int startx;
		int starty;
		int animateHeight;
		int animateWidth;
		double speed;
	};

	struct Sprite {
		std::string name;
		int x;
		int y;
	};

	struct SpriteSheet {
		std::string source;
		int spriteHeight;
		int spriteWidth;
		Sprite* sprites;
		int spritesSize;
		AnimatedObject* animated;
		int animatedSize;
	};

	struct SpriteSheetSet {
		SpriteSheet* sprites;
		int spriteCount;
	};

	enum SpriteType {
		SPRITE_TILES,
		SPRITE_ITEMS,
		SPRITE_CREATURES,
		SPRITE_PLAYER
	};

	static SpriteSheetSet parse(std::string source, SpriteType type);
private:
	struct Impl;
	static Impl impl;
};