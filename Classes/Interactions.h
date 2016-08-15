#include <string>
#include "cocos2d.h"
#include "Parser/Responces.h"
#include "Lua/LuaTileActions.h"
#include "Lua/LuaCreatureActions.h"
#include "SelectionWheel.h"
#include "MyMath.h"
#include "Creatures.h"

USING_NS_CC;

class Interact {
public:
	static void InteractMap(Vec2f tileCoordinates);
	static bool isMovable(string tile, int layerIndex);

	//List of different moves, return time needed to complete move
	static double moveOn(Vec2f tileCoordinates);
	static double moveOn(Vec2f tileCoordinates, double speed);
	static double moveTo(Vec2f tileCoordinates, int distance, double speed);
	static double moveTo(Vec2f tileCoordinates, int distance);
	static double moveTo(Vec2f tileCoordinates);

	static void runInteraction(Interaction, Vec2i tileCoordinates);

	static void update(double delta);

	static void destroyChoices();
private:

	enum InteractionType {
		INTERACT_CREATURE,
		INTERACT_TILE
	};

	static void queryUser(list<Interaction>, Vec2i tileCoordinates);
	static ActionList getPossibleActions(ActionList, InteractionType);

	static void runCreatureInteractions(Creature* creature);

	static ActionMap interactionsRepo;

	static void cancelCommand();
	static void scheduleCommand(std::string, double time);
	static LuaTileActions m_currentLuaParser;
	static LuaCreatureActions m_currentCreatureParser;

	// Choices
	static SelectionWheel* m_choices;
	static bool m_choicesIsDestroyed;
	static bool m_choicesNeedToBeDestroyed;
	static Vec2i m_selectedCoordinates;

	static std::string m_scheduledCommand;
	static double m_scheduledCommandTime;
	static double m_scheduledCommandTimeCount;
	static bool m_isCommandScheduled;
};
