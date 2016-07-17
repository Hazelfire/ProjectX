#include "Interactions.h"
#include <unordered_map>
#include "Player.h"
#include "cocos2d.h"////////////////>
#include "Arena.h"
#include "PathFinder.h"///////////////>
#include "ScriptLoader.h"
#include "ResourceMacros.h"
#include "Multiplayer/XClient.h"
#include "Multiplayer/PuppetMaster.h"/*				
=====================================================\
'''''''''''''''''''''''''''''''''''''''|	|		  \ 				*Preparing for takeoff*
.......................................|	|         /					- Shamefully By Sam Nolan
=====================================================/
#include "Multiplayer\PuppetMaster.h"
#include "Multiplayer/XClient.h"
#include "ResourceMacros.h"
#include "ScriptLoader.h"
#include "PathFinder.h"///////////////>
#include "Arena.h"
#include "cocos2d.h"////////////////>
#include "Player.h"
#include <unordered_map>
#include "Interactions.h"
*/
#define LOAD_REPO() if (interactionsRepo.tiles.empty()) { \
					interactionsRepo = InteractionsParser::parse(ScriptLoader::loadXmlScript(ScriptLoader::XML_INTERACTIONS));\
					}

USING_NS_CC;
using namespace std;

LuaTileActions Interact::m_currentLuaParser;
ActionMap Interact::interactionsRepo;
std::string Interact::m_scheduledCommand;
double Interact::m_scheduledCommandTime;
double Interact::m_scheduledCommandTimeCount;
bool Interact::m_isCommandScheduled;
SelectionWheel* Interact::m_choices;
bool Interact::m_choicesNeedToBeDestroyed = false;
bool Interact::m_choicesIsDestroyed = true;
Vec2i Interact::m_selectedCoordinates;

void Interact::InteractMap(float x, float y) {
	// load the Repo if not already
	LOAD_REPO();
	Mortal* player = PuppetMaster::getPlayerAt(Vec2f(x, y));
	if (player && player->getPlayerIndex() != XClient::getPlayerIndex()) {
		// it's not me, so it must be a puppet, cast
		Puppet* puppet = (Puppet*)player;
		puppet->showInteractions();
	}

	Creature* creature = Creature::getCreatureAt(Vec2f(x, y));
	if (creature) {
		runCreatureInteractions(creature);
	}

	string tileName = Arena::getMapInstance()->getTileNameAt(Vec2i(x,y));
	ActionList possibleActions = getPossibleActions(interactionsRepo.tiles[tileName], INTERACT_TILE);
	if (possibleActions.isMovable) {
		m_selectedCoordinates = Vec2i(-1, -1);
		moveOn(x, y);
		return;
	}
	if (possibleActions.options.size() == 1) {
		//CCLOG("3");
		runInteraction(possibleActions.options.front(), x, y);
	}
	else if (possibleActions.options.size() > 1) {
		queryUser(possibleActions.options, x, y);
	}
}

ActionList Interact::getPossibleActions(ActionList set, InteractionType type) {
	ActionList re;
	re.isMovable = set.isMovable;
	for (auto currentInteraction : set.options) {
		if (type == INTERACT_TILE) {
			LuaTileActions interpreter;
			if (currentInteraction.ifConditional.empty())
				re.options.push_back(currentInteraction);
			else if (interpreter.fulfills(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_INTERACTIONS), currentInteraction.ifConditional))
				re.options.push_back(currentInteraction);
		} else{
			LuaCreatureActions interpreter;
			if (currentInteraction.ifConditional.empty())
				re.options.push_back(currentInteraction);

			else if (interpreter.fulfills(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_CREATURE_ACTIONS), currentInteraction.ifConditional))
				re.options.push_back(currentInteraction);

		}
	}
	return re;
}

void Interact::runInteraction(Interaction action, int x, int y) {
	m_selectedCoordinates = Vec2(-1, -1);
	m_currentLuaParser.init(Vec2i(x,y));
	if (action.instant) {
		m_currentLuaParser.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_INTERACTIONS),action.command);
	}
	else {
		double time = moveTo(x, y);
		scheduleCommand(action.command, time);
	}
}

void Interact::queryUser(list<Interaction> options, int x, int y) {

	// If the item is already selected, do not bring it up
	if (m_selectedCoordinates == Vec2i(x, y)) return;
	m_selectedCoordinates = Vec2i(x, y);

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
		m_choices->setSectionCallback(i, [currentInteractionObject,x,y]() {
			Interact::destroyChoices();
			Interact::runInteraction(currentInteractionObject,x,y);
			
		});
		i++;
	}

	Arena::getMapInstance()->placeNode(Vec2i(x,y),m_choices);
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
double Interact::moveOn(int x, int y) {
	return moveTo(x, y, 0);
}

double Interact::moveTo(int x, int y) {
	return moveTo(x, y, 1);
}

double Interact::moveTo(int x, int y, int distance) {
	return moveTo(x,y,distance,Player::getInstance()->getMovementSpeed());
}

double Interact::moveOn(int x, int y, double speed) {
	return moveTo(x, y, 0, Player::getInstance()->getMovementSpeed());
}

double Interact::moveTo(int x, int y, int distance, double speed){
	// if the player has moved, other scheduled commands can be canceled
	cancelCommand();
	auto player = Player::getInstance();

	XClient::getInstance()->movePlayer(Vec2i(x, y), distance, speed);

	return player->moveTo(Vec2i(x,y), distance, speed);
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
