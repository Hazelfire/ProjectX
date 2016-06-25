#include "XClient.h"
#include "base/CCConsole.h"
#include "Save.h"
#include "XProtocol.hpp"
#include "Parser/StringOperations.h"
#include <math/CCMath.h>
#include "PuppetMaster.h"
#include "Arena.h"
#include "../UIScenes/ErrorScreen.h"
#include "Creatures.h"
#include "Inventory.h"

#include "Sources/HashDirectory.h"

// a lot of zeroes does not neccesarily mean a big number
int XClient::m_playerIndex = 0000000000000000000000000000000;

#define GAME_VERSION_NO 1

XClient XClient::m_clientInstance;

void XClient::init(PlayerInformation playerInfo, Lobby* lobby) {
	m_playerInfo = playerInfo;
	m_lobby = lobby;
}

void XClient::onConnect() {
	CCLOG("Successful Connection");

	// connection message and name
	XMessage<MCONNECT> message(m_playerInfo.playerName, m_playerInfo.classIndex);

	sendMessage(message);

	/*uint32_t hash = hashDirectory("res");

	XMessage<MSOURCE_HASH> hashOfSources(hash, "res");

	sendMessage(hashOfSources);*/
}

void XClient::startGame() {
	int seed = cocos2d::RandomHelper::random_int(0, 65535);

	XMessage<MSTART_GAME> startMessage(seed);
	sendMessage(startMessage);
}

// Complete message handler for the XClient, I would highly recomend collapsing all of the if statements

void XClient::onMessage(std::string& data) {
	XMessage<0> message = data;

	// These commands are only available when we are not in-game
	if (!m_isGameState) {
		// If the data that is sent is for connection (player name and class)
		if (message.isType(MCONNECT) || message.isType(MCONNECTED_PLAYER)) {

			XMessage<MCONNECT> connectMessage = message;
			PlayerInformation playerInfo;
			connectMessage.get(playerInfo.playerName, playerInfo.classIndex);

			m_lobby->onPlayerConnect(playerInfo);
		}

		// recieving server information
		else if (message.isType(MSERVER_INFO)) {

			XMessage<MSERVER_INFO> serverInfoMessage = message;
			std::string hostsName, map;
			serverInfoMessage.get(hostsName, map);


			m_mapName = map;
			m_lobby->onServerInformation(hostsName, map);
		}

		// disconnect message
		else if (message.isType(MDISCONNECT)) {
			XMessage<MDISCONNECT> disconnectMessage = message;
			unsigned char playerIndex;
			disconnectMessage.get(playerIndex);
			m_lobby->onPlayerDisconnect(playerIndex);
		}

		// asignment of a player index
		else if (message.isType(MINDEX_ASIGN)) {
			XMessage<MINDEX_ASIGN> indexAsignMessage = message;

			unsigned char index;
			indexAsignMessage.get(index);

			m_playerIndex = index;
		}

		// game started
		else if (message.isType(MSTART_GAME)) {
			XMessage<MSTART_GAME> start = message;
			unsigned int seed;
			start.get(seed);

			PuppetMaster::StartInfo startInformation;
			startInformation.mapName = m_mapName;
			startInformation.playerInformation = { m_lobby->m_connectedPlayers.begin(), m_lobby->m_connectedPlayers.end() };
			startInformation.mapSeed = seed;
			startInformation.playerIndex = m_playerIndex;
			startInformation.playerCount = m_lobby->m_connectedPlayers.size();

			Vec2i position = PuppetMaster::start(startInformation);

			XMessage<MSPAWNED> spawnMessage(m_playerIndex, position);

			sendMessage(spawnMessage);

			m_isGameState = true;
		}
	}

	else {

		// If a player has spawned in the world, register it with puppet master
		if (message.isType(MSPAWNED)) {
			XMessage<MSPAWNED> spawnMessage = message;
			Vec2i spawnPosition;
			unsigned char playerIndex;
			spawnMessage.get(playerIndex, spawnPosition);

			// if the player that they are talking about is not you
			if (playerIndex != m_playerIndex) {
				PuppetMaster::spawnPlayer(playerIndex, spawnPosition);
			}
		}

		// If a player has moved
		else if (message.isType(MPLAYER_MOVE_TO)) {
			XMessage<MPLAYER_MOVE_TO> moveMessage = message;
			Vec2i position;
			unsigned char playerIndex, distance;
			float speed;
			moveMessage.get(playerIndex, position, distance, speed);

			// If the message is not me moving
			if (playerIndex != m_playerIndex)
				PuppetMaster::movePlayer(playerIndex, position, distance, speed);
		}

		// If a change has been made to the map
		else if (message.isType(MMAP_STATE_CHANGE)) {
			XMessage<MMAP_STATE_CHANGE> stateChangeMessage = message;

			Vec2i position;
			int state;

			stateChangeMessage.get(position, state);
			
			Arena::getMapInstance()->setTileState(position, state);
		}

		// if a player has diconnected
		else if (message.isType(MDISCONNECT)) {
			XMessage<MDISCONNECT> disconnectMessage = message;
			unsigned char playerIndex;
			disconnectMessage.get(playerIndex);
			
			PuppetMaster::despawnPlayer(playerIndex);
		}

		// If a creature has moved
		else if (message.isType(MCREATURE_MOVE_TO)) {
			XMessage<MCREATURE_MOVE_TO> creatureMoveMessage = message;
			unsigned int creatureIndex;
			Vec2i position;
			unsigned char distance;
			float speed;
			creatureMoveMessage.get(creatureIndex, position, distance, speed);
			
			Creature* creature = Creature::getWithId(creatureIndex);
			creature->moveTo(position, distance, speed);
		}

		// if a creature has spawned
		else if (message.isType(MCREATURE_SPAWN)) {
			XMessage<MCREATURE_SPAWN> creatureSpawnMessage = message;
			std::string creatureName;
			Vec2i position;
			creatureSpawnMessage.get(creatureName, position);
			Creature::spawnCreature(creatureName, position);
		}

		// someone has given me an item! how nice!
		else if (message.isType(MGIVE_PLAYER_ITEM)) {
			XMessage<MGIVE_PLAYER_ITEM> giveMessage = message;
			unsigned char playerIndex;
			std::string itemName;
			unsigned int quantity;
			giveMessage.get(playerIndex, itemName, quantity);
			Inventory::giveItem(itemName, quantity);
		}

		// someone has taken an item from me! how cruel!
		else if (message.isType(MGIVE_PLAYER_ITEM)) {
			XMessage<MGIVE_PLAYER_ITEM> takeMessage = message;
			unsigned char playerIndex;
			std::string itemName;
			unsigned int quantity;
			takeMessage.get(playerIndex, itemName, quantity);
			Inventory::takeItem(itemName, quantity);
		}

		// Someone has changed the map properties, we have to keep with the flow and update ours!
		else if (message.isType(MSET_TILE_PROPERTIES)) {
			XMessage<MSET_TILE_PROPERTIES> setPropertiesMessage = message;
			Vec2i tilePosition;
			std::string properties;
			setPropertiesMessage.get(tilePosition, properties);

			Arena::getMapInstance()->setTileProperies(tilePosition, properties);
		}
	}
}

void XClient::movePlayer(Vec2i position, unsigned char distance, float speed) {
	if (this) {
		XMessage<MPLAYER_MOVE_TO> moveMessage(m_playerIndex, position, distance, speed);

		sendMessage(moveMessage);
	}
}

XClient* XClient::getInstance() {
	return &m_clientInstance;
}

void XClient::changeMap(Vec2i position, int state) {
	if (this) {
		XMessage<MMAP_STATE_CHANGE> mapChangeMessage(position, state);
		sendMessage(mapChangeMessage);
	}
}

void XClient::onClose() {
	Director::getInstance()->replaceScene(ErrorScreen::create("Connection closed by server"));
}

void XClient::onError(std::string error) {
	if (m_isGameState) {
		Director::getInstance()->replaceScene(ErrorScreen::create(error));
	}
	else {
		m_lobby->onError(error);
	}
}

int XClient::getPlayerIndex() {
	return m_playerIndex;
}

void XClient::moveCreature(int creatureIndex, Vec2i position, int distance, float speed) {
	if (this) {
		XMessage<MCREATURE_MOVE_TO> creatureMovedMessage(creatureIndex, position, distance, speed);
		sendMessage(creatureMovedMessage);
	}
}

void XClient::spawnCreature(std::string creatureName, Vec2i location) {
	if (this) {
		XMessage<MCREATURE_SPAWN> creatureSpawn(creatureName, location);
		sendMessage(creatureSpawn);
	}
}

void XClient::giveItem(unsigned char playerIndex, std::string itemName, unsigned int quantity) {
	if (this) {
		XMessage<MGIVE_PLAYER_ITEM> giveItem(playerIndex, itemName, quantity);
		sendMessage(giveItem);
	}
}

void XClient::takeItem(unsigned char playerIndex, std::string itemName, unsigned int quantity) {
	if (this) {
		XMessage<MTAKE_ITEM> takeItem(playerIndex, itemName, quantity);
		sendMessage(takeItem);
	}
}

void XClient::setTileProperties(Vec2i tileLocation, std::string properties) {
	if (this) {
		XMessage<MSET_TILE_PROPERTIES> setTileProperties(tileLocation, properties);
		sendMessage(setTileProperties);
	}
}
