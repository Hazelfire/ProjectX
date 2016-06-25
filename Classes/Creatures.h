#pragma once
#include "cocos2d.h"
#include "Parser/CreaturesParser.h"
#include <unordered_map>
#include "MyMath.h"
#include "Lua/LuaCreature.h"
#include <stack>
#include "Animal.h"
USING_NS_CC;

struct SourceInformation {
	std::string source;
	int x;
	int y;
	int width;
	int height;
};

typedef std::unordered_map<std::string, SourceInformation> SpriteSheetMap;

class Creature : public Animal {
public:
	// Creates a creatures, please use spawnCreature instead
	static Creature* create(std::string creatureName);

	// Spawns a creature at a specific location
	static Creature* spawnCreature(std::string creatureName, Vec2 location, bool isNew = true);
	
	// Gets a reference to a creature based upon the creature's ID
	static Creature* getWithId(short int ID);
	
	// Gets the creature that is at the location
	static Creature* getCreatureAt(Vec2f location);

	// Creates a state that calls based on how far the player is away from the creature
	void newProximityState(double distance, std::string stateName);

	// Creates a state that calls every frame
	void newUpdateState(std::string stateName);

	// Creates a state that calls every [clock] seconds
	void newClockState(double clock,std::string stateName);
	
	// Searches a state by name and deletes it, if no a state, nothing happens
	void deleteState(std::string);

	// despawns the creature
	void despawn();

	// gets the gid of the creature
	int getGID();

	// override moveTo (sending everyone else changes)
	virtual double moveOn(Vec2i position);
	virtual double moveOn(Vec2i position, double speed);
	virtual double moveTo(Vec2i position, int distance, double speed, bool isNew = true);
	virtual double moveTo(Vec2i position, int distance);
	virtual double moveTo(Vec2i position);

	// Gets the name, AI and tags of the creature
	std::string getCreatureName();
	std::string getCreatureAI();
	std::list<std::string> getCreatureTags();
	bool hasTag(std::string tag);
	
	std::string getCreatureProperties();
	void setCreatureProperties(std::string);

private:
	std::string m_name;

	virtual void update(float delta);

	void callStates(float delta);

	virtual bool init(std::string creatureName);

	bool proxStateRequirement(int& playerIndex,int distance);
	bool clockStateRequirement(int count);

	struct State {
		bool operator==(State other) { return (callback == other.callback); }
		bool isActive = false;
		std::string callback;
	};

	struct ProximityState : public State{
		double distance;
	};
	
	typedef State UpdateState;

	struct ClockState : public State {
		double timeCount =0;
		double clock;
	};

	struct CreatureState{
		std::list<ProximityState> proxStates;
		std::list<UpdateState> updateStates;
		std::list<ClockState> clockStates;
	};

	bool m_active = true;

	std::stack<std::string> m_scheduledDeletes;
	CreatureInformation m_creatureInfo;
	int m_gid;
	LuaCreature m_creatureAI;
	CreatureState m_registeredStates;

	std::string m_propertyTable = "{}";
	
	static int registerCreature(Creature* reference);
	static void unregisterCreature(int gid);

	static CreatureList m_creatureList;
	static SpriteSheetMap m_spriteMap;
	static std::unordered_map<short int, Creature*> m_creatureReferences;
};