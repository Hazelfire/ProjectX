#include "AnimatedSprite.h"


AnimatedSprite* AnimatedSprite::create(ConstructAnimateInformation information) {
	AnimatedSprite* sprite = new(std::nothrow) AnimatedSprite();
	if (sprite->init(information)) {
		sprite->autorelease();
		return sprite;
	}
	else {
		delete sprite;
		sprite = nullptr;
		return nullptr;
	}
	return sprite;
}

bool AnimatedSprite::init(ConstructAnimateInformation information) {
	m_animatedObject = information;

	m_currentSprite = Sprite::create(information.source, CC_RECT_PIXELS_TO_POINTS(Rect(((float)information.spriteWidth*information.startx), ((float)information.spriteHeight* information.starty), (double)information.spriteWidth, (double)information.spriteHeight)));

	m_currentSprite->setAnchorPoint(Vec2(0, 0));
	addChild(m_currentSprite);
	scheduleUpdate();
	setContentSize(Size(information.spriteWidth, information.spriteHeight));
	return true;
}

void AnimatedSprite::update(float delta) {
	if ((int)m_timeCount / m_animatedObject.speed != (int)((m_timeCount + delta) / m_animatedObject.speed)) {
		removeAllChildren();
		m_currentSprite = Sprite::create(m_animatedObject.source,
			CC_RECT_PIXELS_TO_POINTS(Rect(((((int)(m_timeCount / m_animatedObject.speed) / m_animatedObject.height) % m_animatedObject.width) + m_animatedObject.startx) * m_animatedObject.spriteWidth,
				(((int)(m_timeCount / m_animatedObject.speed) % m_animatedObject.height) + m_animatedObject.starty) * m_animatedObject.spriteHeight,
				m_animatedObject.spriteWidth,
				m_animatedObject.spriteHeight)));
		m_currentSprite->setAnchorPoint(Vec2(0, 0));
		addChild(m_currentSprite);
		setContentSize(m_currentSprite->getContentSize());
		m_currentSprite->getTexture()->setAliasTexParameters();
	}
	m_timeCount += delta;
}