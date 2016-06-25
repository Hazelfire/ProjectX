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
		SpriteSheet* tiles;
		int tilesSize;
		SpriteSheet* items;
		int itemsSize;
		SpriteSheet* creatures;
		int creaturesSize;
		SpriteSheet* players;
		int playersSize;
	};


	static SpriteSheetSet parse(std::string source);
private:
	struct Impl;
	static Impl impl;
};