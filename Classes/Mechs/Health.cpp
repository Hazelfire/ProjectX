#include "Health.h"
#include "ResourceMacros.h"

using namespace cocos2d;

Health* Health::m_healthInstance;

Health* Health::create() {
	Health* re = new (std::nothrow) Health();
	if (re && re->init()) {
		re->autorelease();
		m_healthInstance = re;
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool Health::init() {
	Sprite* healthFrame = Sprite::create(HEALTH_FRAME);
	healthFrame->setAnchorPoint(Vec2(0.5, 0.5));
	healthFrame->getTexture()->setAliasTexParameters();
	healthFrame->setScale(3);
	addChild(healthFrame);

	Sprite* emptyHealthBack = Sprite::create(HEALTH_EMPTY_IMAGE);
	emptyHealthBack->setAnchorPoint(Vec2(0.5, 0.5));
	emptyHealthBack->getTexture()->setAliasTexParameters();
	emptyHealthBack->setScale(3);
	addChild(emptyHealthBack);

	ClippingRectangleNode* healthSlider = ClippingRectangleNode::create(Rect(Vec2::ZERO, emptyHealthBack->getContentSize() * 3));
	healthSlider->setContentSize(emptyHealthBack->getContentSize() * 3);
	healthSlider->setAnchorPoint(Vec2(0.5, 0.5));
	healthSlider->setName("Health Slider");
	addChild(healthSlider);

	Sprite* fullHealth = Sprite::create(HEALTH_FULL_IMAGE);
	fullHealth->getTexture()->setAliasTexParameters();
	fullHealth->setScale(3);
	fullHealth->setPosition(healthSlider->getContentSize()/2);
	healthSlider->addChild(fullHealth);

	setContentSize(healthFrame->getContentSize() * 3);

	return true;
}

Health* Health::getInstance() {
	return m_healthInstance;
}

double Health::getHealth() {
	return m_health;
}

void Health::heal(double health) {
	m_health += health;
	if (m_health < 0) {
		m_health = 0.0;
	}
	else if (m_health > 100) {
		m_health = 100;
	}

	// update Graphic
	ClippingRectangleNode* healthSlider = (ClippingRectangleNode*)getChildByName("Health Slider");
	healthSlider->setClippingRegion(Rect(Vec2::ZERO, Size(healthSlider->getContentSize().width * (m_health / 100.0), healthSlider->getContentSize().height)));
}

void Health::damage(double health) {
	heal(-health);
}

void Health::setHealth(double health) {
	heal(health - m_health);
}
