#include "Interactions.h"
#include <unordered_map>
#include "Player.h"
#include "cocos2d.h"
#include "Arena.h"
#include "PathFinder.h"
#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include "Multiplayer/XClient.h"
#include "Multiplayer/PuppetMaster.h"


#define LOAD_REPO() if (interactionsRepo.tiles.empty()) { \
					interactionsRepo = InteractionsParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_INTERACTIONS));\
					}

USING_NS_CC;
using namespace std;

LuaActions Interact::m_currentLuaParser;
ActionMap Interact::interactionsRepo;
std::string Interact::m_scheduledCommand;
double Interact::m_scheduledCommandTime;
double Interact::m_scheduledCommandTimeCount;
bool Interact::m_isCommandScheduled;
SelectionWheel* Interact::m_choices;
bool Interact::m_choicesNeedToBeDestroyed = false;
bool Interact::m_choicesIsDestroyed = true;
Vec2i Interact::m_selectedCoordinates;

void Interact::InteractMap(Vec2f tileCoordinates) {
	// load the Repo if not already
	LOAD_REPO();
	Mortal* player = PuppetMaster::getPlayerAt(tileCoordinates);
	if (player && player->getPlayerIndex() != XClient::getPlayerIndex()) {
		// it's not me, so it must be a puppet, cast
		Puppet* puppet = (Puppet*)player;
		puppet->showInteractions();
	}

	Creature* creature = Creature::getCreatureAt(tileCoordinates);
	if (creature) {
		runCreatureInteractions(creature);
	}

	string tileName = Arena::getMapInstance()->getTileNameAt(tileCoordinates);
	ActionList possibleActions = getPossibleActions(interactionsRepo.tiles[tileName], INTERACT_TILE);
	if (possibleActions.isMovable) {
		m_selectedCoordinates = Vec2i(-1, -1);
		moveOn(tileCoordinates);
		return;
	}
	if (possibleActions.options.size() == 1) {

		runInteraction(possibleActions.options.front(), tileCoordinates);
	}
	else if (possibleActions.options.size() > 1) {
		queryUser(possibleActions.options, tileCoordinates);
	}
}

ActionList Interact::getPossibleActions(ActionList set, InteractionType type) {
	ActionList re;
	re.isMovable = set.isMovable;
	for (std::list<Interaction>::iterator currentInteraction = set.options.begin(); currentInteraction != set.options.end(); currentInteraction++) {
		LuaGame interpreter;
		if (currentInteraction->ifConditional.empty()) 
			re.options.push_back(*currentInteraction);
		else if (interpreter.fulfills(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_INTERACTIONS), currentInteraction->ifConditional))
			re.options.push_back(*currentInteraction);
	}
	return re;
}

void Interact::runInteraction(Interaction action, Vec2i tileCoordinates) {
	m_selectedCoordinates = Vec2(-1, -1);
	m_currentLuaParser.init(tileCoordinates);
	if (action.instant) {
		m_currentLuaParser.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_INTERACTIONS),action.command);
	}
	else {
		double time = moveTo(tileCoordinates);
		scheduleCommand(action.command, time);
	}
}

void Interact::queryUser(list<Interaction> options, Vec2i tileCoordinates) {

	// If the item is already selected, do not bring it up
	if (m_selectedCoordinates == Vec2i(tileCoordinates)) return;
	m_selectedCoordinates = Vec2i(tileCoordinates);

	destroyChoices();
	m_choicesIsDestroyed = false;
	std::list<std::string> selectionWheelOptions;
	for (std::list<Interaction>::iterator currentInteraction = options.begin(); currentInteraction != options.end(); currentInteraction++) {
		selectionWheelOptions.push_back(currentInteraction->name);
	}
	m_choices = SelectionWheel::create(selectionWheelOptions);

	int i = 0;
	for (std::list<Interaction>::iterator currentInteraction = options.begin(); currentInteraction != options.end(); currentInteraction++) {
		Interaction currentInteractionObject = *currentInteraction;
		m_choices->setSectionCallback(i, [currentInteractionObject,tileCoordinates]() {
			Interact::destroyChoices();
			Interact::runInteraction(currentInteractionObject, tileCoordinates);
			
		});
		i++;
	}

	Arena::getMapInstance()->placeNode(tileCoordinates,m_choices);
	// a very large number. Gets it to the top of the screen
	m_choices->setLocalZOrder(200000000);
}

void Interact::destroyChoices() {
	if (!m_choicesIsDestroyed) {
		m_choicesIsDestroyed = true;
		m_choicesNeedToBeDestroyed = true;
	}
}

bool Interact::isMovable(string tileName){
	LOAD_REPO();
	// If the tile in question is not a tile
	if (tileName.empty()) {
		return true;
	}
	return interactionsRepo.tiles[tileName].isMovable;
}

//List of interactions
double Interact::moveOn(Vec2f tileCoordinates) {
	return moveTo(tileCoordinates, 0);
}

double Interact::moveTo(Vec2f tileCoordinates) {
	return moveTo(tileCoordinates, 1);
}

double Interact::moveTo(Vec2f tileCoordinates, int distance) {
	return moveTo(tileCoordinates,distance,Player::getInstance()->getMovementSpeed());
}

double Interact::moveOn(Vec2f tileCoordinates, double speed) {
	return moveTo(tileCoordinates, 0, Player::getInstance()->getMovementSpeed());
}

double Interact::moveTo(Vec2f tileCoordinates, int distance, double speed){
	// if the player has moved, other scheduled commands can be canceled
	cancelCommand();
	auto player = Player::getInstance();

	XClient::getInstance()->movePlayer(tileCoordinates, distance, speed);

	return player->moveTo(tileCoordinates, distance, speed);
}

void Interact::scheduleCommand(std::string command, double time) {
	m_scheduledCommand = command;
	m_scheduledCommandTime = time;
	m_scheduledCommandTimeCount = 0;
	m_isCommandScheduled = true;
}

void Interact::cancelCommand() {
	// coz y not?
	destroyChoices
		(
			)
		;

	m_scheduledCommand = "";
	m_scheduledCommandTime = 0;
	m_scheduledCommandTimeCount = 0;
	m_isCommandScheduled = false;
}

void Interact::update(double delta) {
	if (m_choicesNeedToBeDestroyed) {
		m_choicesNeedToBeDestroyed = false;
		m_choices->removeFromParentAndCleanup(true);
	}
	if (m_isCommandScheduled) {
		m_scheduledCommandTimeCount += delta;
		if (m_scheduledCommandTimeCount > m_scheduledCommandTime) {
			m_currentLuaParser.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_INTERACTIONS),m_scheduledCommand);
			cancelCommand();
		}
	}
}

void Interact::runCreatureInteractions(Creature* creature) {
	std::string creatureName = creature->getCreatureName();
	ActionList actions = getPossibleActions(interactionsRepo.creatures[creatureName], INTERACT_CREATURE);
	if (actions.options.size() == 1) {
		cancelCommand();

	}
}
