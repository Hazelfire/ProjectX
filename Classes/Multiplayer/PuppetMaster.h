#pragma once

#include "GameInformation.h"
#include <vector>
#include "MyMath.h"
#include "Puppet.h"

// Controls multiplayer functionality of the arena.
// A person who is not the actual player of the current game will be called a "puppet"
// This is because these puppets are not actually players in a game sense, they are entities
// That are controlled by the real player entity on another computer
// Hence, this class is called the PuppetMaster

class PuppetMaster {
public:

	// information required to initiate the puppet master
	struct StartInfo {
		std::string mapName;
		int mapSeed;
		std::vector<PlayerInformation> playerInformation;
		int playerIndex;
		int playerCount;
	};

	// starts game, returns starting player coordiantes
	static Vec2i start(StartInfo info);

	// instanciating players into the game
	static void spawnPlayer(int playerIndex, Vec2i playerPosition);
	static void despawnPlayer(int playerIndex);

	static Mortal* getPlayer(int playerIndex);

	static void movePlayer(unsigned char playerIndex, Vec2i position, unsigned char distance, float speed);

	static Mortal* getPlayerAt(Vec2f location);
private:
	static std::vector<PlayerInformation> m_playerInformation;
	static std::vector<Mortal*> m_mortals;
};