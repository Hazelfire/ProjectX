#include "Creatures.h"
#include "Arena.h"
#include "PathFinder.h"
#include "ScriptLoader.h"
#include "SpriteLoader.h"
#include "Multiplayer/PuppetMaster.h"
#include "Multiplayer/XClient.h"
CreatureList Creature::m_creatureList;
std::unordered_map<short int, Creature*> Creature::m_creatureReferences;

#define NOT_ACTIVE() if(!m_active) return
Creature* Creature::create(std::string creatureName) {
	Creature* creature = new(std::nothrow)Creature();
	if (creature->init(creatureName)) {
		creature->autorelease();
		return creature;
	}
	else {
		delete creature;
		creature = nullptr;
		return nullptr;
	}
}

bool Creature::init(std::string creatureName) {
	// Registering creature
	m_gid = registerCreature(this);

	scheduleUpdate();

	// If the creature list has not already been parsed
	if (m_creatureList.creatures.size() == 0) {
		m_creatureList = CreatureParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_CREATURE));
	}

	m_name = creatureName;

	m_creatureInfo = m_creatureList.creatures.at(creatureName);
	
	Animal::init(m_creatureInfo.movements,SPRITE_CREATURE, m_creatureInfo.movementSpeed);

	m_creatureAI.init(m_gid);
	m_creatureAI.run(ScriptLoader::loadSeperateLuaScript(ScriptLoader::LUA_CREATURES).at(m_creatureInfo.ai));
	return true;
}

int Creature::registerCreature(Creature* reference) {
	short int gid = 0;

	// While there is a creature in the gid's spot
	while (m_creatureReferences.find(gid) != m_creatureReferences.end()) {
		gid++;
	}

	//this spot has no creature, store creature;
	m_creatureReferences[gid] = reference;
	
	//return the gid for future reference
	return gid;
}

void Creature::unregisterCreature(int gid) {
	m_creatureReferences.erase(gid);
}

Creature* Creature::getWithId(short int ID) {

	// If we do not have a creature at that ID
	if (m_creatureReferences.find(ID) == m_creatureReferences.end()) {
		return nullptr;
	}

	// Return creature
	return m_creatureReferences[ID];
}

Creature* Creature::spawnCreature(std::string creatureName, Vec2 location, bool isNew) {
	Creature* creature = Creature::create(creatureName);
	Arena::getMapInstance()->placeNode(location, creature);
	return creature;
}

void Creature::newProximityState(double distance, std::string stateName) {
	ProximityState state;
	state.callback = stateName;
	state.distance = distance;
	m_registeredStates.proxStates.push_back(state);
}

void Creature::newUpdateState(std::string stateName) {
	UpdateState state;
	state.callback = stateName;
	m_registeredStates.updateStates.push_back(state);
}

void Creature::newClockState(double clock, std::string stateName) {
	ClockState state;
	state.callback = stateName;
	state.clock = clock;
	m_registeredStates.clockStates.push_back(state);
}

void Creature::update(float delta) {
	Animal::update(delta);

	callStates(delta);

	setContentSize(Size(32,24));
	setAnchorPoint(Vec2(0, 1));

	if (!m_active) {
		unregisterCreature(m_gid);
		unscheduleUpdate();
		removeFromParent();
	}
}

void Creature::deleteState(std::string stateName) {
	m_scheduledDeletes.push(stateName);
}

void Creature::callStates(float delta){
	// Handles the deletion of states, you can't delete a state while it is running, so instead it is scheduled, same thing for despawn
	while (!m_scheduledDeletes.empty()) {
		std::string stateName = m_scheduledDeletes.top();
		m_scheduledDeletes.pop();
		ProximityState proxStateLookingFor;
		proxStateLookingFor.callback = stateName;

		m_registeredStates.proxStates.remove(proxStateLookingFor);

		UpdateState updateStateLookingFor;
		updateStateLookingFor.callback = stateName;

		m_registeredStates.updateStates.remove(updateStateLookingFor);

		ClockState clockStateLookingFor;
		clockStateLookingFor.callback = stateName;

		m_registeredStates.clockStates.remove(clockStateLookingFor);
	}


	for (std::list<ProximityState>::iterator currentProximityState = m_registeredStates.proxStates.begin(); currentProximityState != m_registeredStates.proxStates.end(); currentProximityState++) {
		int playerIndex;
		if (proxStateRequirement(playerIndex, currentProximityState->distance)) {
			if (!currentProximityState->isActive) {
				currentProximityState->isActive = true;
				m_creatureAI.callWithPlayer("begin" + currentProximityState->callback, playerIndex);
			}
			else {
				m_creatureAI.callWithPlayer("is" + currentProximityState->callback, playerIndex);
			}
		}
		else if (currentProximityState->isActive) {
			currentProximityState->isActive = false;
			m_creatureAI.callWithPlayer("end" + currentProximityState->callback, playerIndex);
		}
	}
	for (std::list<UpdateState>::iterator currentUpdateState = m_registeredStates.updateStates.begin(); currentUpdateState != m_registeredStates.updateStates.end(); currentUpdateState++) {
		m_creatureAI.call(currentUpdateState->callback);
	}
	for (std::list<ClockState>::iterator currentClockState = m_registeredStates.clockStates.begin(); currentClockState != m_registeredStates.clockStates.end(); currentClockState++) {
		if (currentClockState->timeCount > currentClockState->clock) {
			m_creatureAI.call(currentClockState->callback);
			currentClockState->timeCount = 0;
		}
		currentClockState->timeCount += delta;
	}
}

void Creature::despawn() {
	m_active = false;
}

bool Creature::proxStateRequirement(int& playerIndex,int distance) {
	playerIndex = Player::getInstance()->getPlayerIndex();
	if (distance > Player::getInstance()->getRealTilePosition().distance(getRealTilePosition()))
		return true;
	else
		return false;
}

bool Creature::clockStateRequirement(int count) {
	return false;
}

int Creature::getGID() {
	return m_gid;
}

double Creature::moveOn(Vec2i position) {
	return moveTo(position, 0);
}

double Creature::moveOn(Vec2i position, double speed) {
	return moveTo(position, 0, speed);
}

double Creature::moveTo(Vec2i position, int distance) {
	return moveTo(position, distance, getMovementSpeed());
}

double Creature::moveTo(Vec2i position) {
	return moveTo(position, 1);
}

double Creature::moveTo(Vec2i position, int distance, double speed, bool isNew) {
	XClient::getInstance()->moveCreature(getGID(), position, distance, speed);
	// If the player cannot see the puppet, just spawn it over
	if (Player::getInstance()->getTilePosition().distance(getTilePosition()) > 30) {
		setTilePosition(position);
	}
	// if the creature is close, actually move it
	else {
		return Animal::moveTo(position, distance, speed);
	}
}


std::string Creature::getCreatureName() {
	return m_name;
}

std::string Creature::getCreatureAI() {
	return m_creatureInfo.ai;
}

std::list<std::string> Creature::getCreatureTags() {
	return m_creatureInfo.tags;
}

bool Creature::hasTag(std::string tag) {
	for (auto currentTag : m_creatureInfo.tags) {
		if (currentTag == tag)
			return true;
	}
	return false;
}

std::string Creature::getCreatureProperties() {
	return m_propertyTable;
}

void Creature::setCreatureProperties(std::string table) {
	m_propertyTable = table;
}

Creature* Creature::getCreatureAt(Vec2f location) {
	for (auto currentCreature : m_creatureReferences) {
		if (currentCreature.second->getRealTilePosition().distance(location) < 2)
			return currentCreature.second;
	}
	return nullptr;
}