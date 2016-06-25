#include "Puppet.h"
#include "Arena.h"
#include "PathFinder.h"
#include "XBMPLabel.h"
#include "SelectionWheel.h"

Puppet* Puppet::create(int classIndex, std::string playerName, int playerIndex) {
	Puppet* re = new (std::nothrow) Puppet();
	if (re && re->init(classIndex, playerName,playerIndex)) {
		re->autorelease();
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool Puppet::init(int classIndex, std::string playerName, int playerIndex) {
	Mortal::init(classIndex, playerName, playerIndex);

	XBMPLabel* playerLabel = XBMPLabel::create(playerName, "Pixelfont", 20, XBMPLabel::CENTER);
	playerLabel->setPosition(Vec2(0, 40));
	addChild(playerLabel);
	
	
	scheduleUpdate();

	return true;
}

void Puppet::update(float delta) {
	Player* player = Player::getInstance();

	Mortal::update(delta);
	// if the player is close
	if (player->getTilePosition().distance(getTilePosition()) < 30) {
		m_active = true;
	}
	else {
		m_active = false;
	}
}

double Puppet::moveTo(Vec2i position, unsigned char distance, float speed) {
	
	// If the player cannot see the puppet, just spawn it over
	if (!m_active) {
		setTilePosition(position);
	}

	// if the player can see the puppet, do a propper move
	else {
		return Animal::moveTo(position, distance, speed);
	}
}

void Puppet::showInteractions() {

}