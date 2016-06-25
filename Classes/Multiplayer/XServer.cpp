#include "CCPlatformMacros.h"
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)


#include "XServer.h"
#include "XProtocol.hpp"
#include "Parser/StringOperations.h"
#include "Sources/HashDirectory.h"
// destroys element and squashes vector
template <class AnyType>
void squashVector(std::vector<AnyType> & vector, int index) {
	for (int i = index; i < vector.size() -1; i++) {
		vector[i] = vector[i + 1];
	}
	vector.resize(vector.size() - 1);
}

void XServer::init(std::string nameHost, std::string map) {
	m_serverInfo.nameHost = nameHost;
	m_serverInfo.map = map;
}


void XServer::onOpen(int player) {

	// send them a record of every player
	for (auto it : m_playerInfo) {
		XMessage<MCONNECTED_PLAYER> message(it.playerName, it.classIndex);

		sendMessage(player, message);
	}
		
	// Let them know what index they are
	XMessage<MINDEX_ASIGN> indexMessage(player);
	sendMessage(player,indexMessage);

	// Let them know about server information (map, host name)
	XMessage<MSERVER_INFO> serverMessage(m_serverInfo.nameHost, m_serverInfo.map);

	sendMessage(player, serverMessage);
}

void XServer::onClose(int playerIndex) {

	// someone disconnected, let everyone know
	squashVector(m_playerInfo, playerIndex);
	
	XMessage<MDISCONNECT> disconnectMessage(playerIndex);
	broadcastMessage(disconnectMessage);

	// send everyone their new index
	for (int currentPlayer = 0; currentPlayer < m_playerInfo.size(); currentPlayer++) {
		XMessage<MINDEX_ASIGN> indexMessage(currentPlayer);
		sendMessage(currentPlayer, indexMessage);
	}
}

void XServer::onMessage(int player, std::string message) {
	XMessage<MNULL> incomingMessage = message;

	// If the player is connecting
	if (incomingMessage.isType(MCONNECT)) {
		if (!m_isGameState) {
			XMessage<MCONNECT> connectMessage = incomingMessage;

			PlayerInformation currentPlayer;
			connectMessage.get(currentPlayer.playerName, currentPlayer.classIndex);
			m_playerInfo.push_back(currentPlayer);
		}

		// If the game has already started, tell them
		else {
			XMessage<MGAME_ALREADY_STARTED> alreadyStartedMessage;
			sendMessage(player, alreadyStartedMessage);
		}
	}

	else if (incomingMessage.isType(MSTART_GAME)) {
		m_isGameState = true;
	}

	// for now, these 2 get special attention, basically meaning that the message gets sent to everyone but the sender
	if (incomingMessage.isType(MCREATURE_MOVE_TO) ||  incomingMessage.isType(MCREATURE_SPAWN)) {
		for (int i = 0; i < getConnectedPlayerCount(); i++) {
			if (i != player) {
				sendMessage(i,incomingMessage);
			}
		}
	} 
	else if (incomingMessage.isType(MGIVE_PLAYER_ITEM)) {
		XMessage<MGIVE_PLAYER_ITEM> giveMessage = incomingMessage;
		unsigned char playerIndex;
		std::string item;
		unsigned int quantity;
		giveMessage.get(playerIndex,item, quantity);
		sendMessage(playerIndex, giveMessage);
	}
	else if (incomingMessage.isType(MTAKE_ITEM)) {
		XMessage<MTAKE_ITEM> takeMessage = incomingMessage;
		unsigned char playerIndex;
		std::string item;
		unsigned int quantity;
		takeMessage.get(playerIndex, item, quantity);
		sendMessage(playerIndex, takeMessage);
	}
	// someone has sent over the hash of their directory, lets check to see if it matches ours
	/*else if (incomingMessage.isType(MSOURCE_HASH)) {
		XMessage<MSOURCE_HASH> sourceHashMessage = incomingMessage;

		unsigned int serverHash = hashDirectory("res");
		unsigned int clientHash;
		std::string directory;
		
		sourceHashMessage.get(clientHash, directory);

		// we have both the hashes now, let's see if they match up!
		if (clientHash == serverHash) {
			// They're matching!! how sweet
		}
		else {
			// Hmm, we have different source directories, let's ask for thier subdirectories
		}

	}*/
	else {

		broadcastMessage(message);
	}
}

#endif