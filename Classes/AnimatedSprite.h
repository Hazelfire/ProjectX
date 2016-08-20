#include "cocos2d.h"
#include "SpriteTypes.h"
USING_NS_CC;


class AnimatedSprite : public Node {
public:
	 
	// Information required for creating the Animated Object
	struct ConstructAnimateInformation {
		Texture2D* texture;
		std::string source;
		int spriteWidth;
		int spriteHeight;
		int startx;
		int starty;
		int width;
		int height;
		double speed;
		SpriteType type;
	};

	static AnimatedSprite* create(ConstructAnimateInformation information);
	virtual bool init(ConstructAnimateInformation information);
	virtual void update(float delta);
private:
	ConstructAnimateInformation m_animatedObject;
	Sprite* m_currentSprite;
	double m_timeCount;
};