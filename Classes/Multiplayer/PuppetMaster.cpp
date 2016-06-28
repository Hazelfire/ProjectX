#include "PuppetMaster.h"
#include "Arena.h"
#include "ScriptLoader.h"
#include "Lua/LuaGame.h"

// declarations of static variables
std::vector<PlayerInformation> PuppetMaster::m_playerInformation;
std::vector<Mortal*> PuppetMaster::m_mortals;

Vec2i PuppetMaster::start(StartInfo info) {
	Arena arena;
	arena.createMulti(info.mapName, info.mapSeed);
	Director::getInstance()->replaceScene(arena.getArenaScene());

	PlayerInformation playerInfo = info.playerInformation[info.playerIndex];

	Player* player = Player::create(playerInfo.classIndex, playerInfo.playerName, info.playerIndex);
	Vec2i tileCoordiantes = arena.placePlayer(player);
	m_mortals.resize(info.playerCount);

	m_mortals[info.playerIndex] = player;

	// Only run startup scripts if you are the host, I may remove startup scripts in the future, they break a lot of logic
	if (info.playerIndex == 0) {
		LuaGame startupScript;
		startupScript.run(ScriptLoader::loadLuaScripts(ScriptLoader::LUA_STARTUP));
	}

	m_playerInformation = info.playerInformation;

	// makes sure that the puppets vector is large enough
	m_mortals.resize(info.playerInformation.size());

	return tileCoordiantes;
}

void PuppetMaster::movePlayer(unsigned char playerIndex,Vec2i position, unsigned char distance, float speed) {
	m_mortals[playerIndex]->moveTo(position, distance, speed);
}

void PuppetMaster::spawnPlayer(int playerIndex, Vec2i playerPosition) {
	Puppet* puppet = Puppet::create(m_playerInformation[playerIndex].classIndex, m_playerInformation[playerIndex].playerName , playerIndex);
	Arena::getMapInstance()->placeNode(playerPosition,puppet);
	m_mortals[playerIndex] = puppet;
}

void PuppetMaster::despawnPlayer(int playerIndex) {
	m_mortals[playerIndex]->removeFromParent();
}

Mortal* PuppetMaster::getPlayer(int playerIndex) {
	// if the game is singlplayer, you dont use the puppetmaster, return the player
	if (m_mortals.size() == 0) {
		return Player::getInstance();
	}
	return m_mortals[playerIndex];
}

Mortal* PuppetMaster::getPlayerAt(Vec2f location) {

	// if singleplayer
	if (m_mortals.size() == 0) {
		if (Player::getInstance()->getTilePosition().distance(location) < 2) {
			return Player::getInstance();
		}
		else {
			return nullptr;
		}
	}
	
	// if multiplayer
	else {
		for (auto player : m_mortals) {
			if (player->getTilePosition().distance(location) < 2)
				return player;
		}
	}

	// no close player found
	return nullptr;
}
