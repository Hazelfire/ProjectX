#include "CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)

#include "MultiplayerServer.h"

#include "XProtocol.hpp"

#include "base/ccMacros.h"

#include <boost/thread.hpp>


	// on connection
void MultiplayerServer::onServerOpen(connection_hdl hdl) {
	m_connections.insert(hdl);

	onOpen(m_connections.size() - 1);
};

void MultiplayerServer::onServerClose(connection_hdl hdl) {
	int index = findIndex(hdl);
	m_connections.erase(hdl);
	onClose(index);
};

// When recieves message
void MultiplayerServer::onServerMessage(connection_hdl hdl, Server::message_ptr msg) {
	int index = findIndex(hdl);
	onMessage(index, msg->get_payload());
}

void MultiplayerServer::run(unsigned short port) {
	m_server.listen(port);
	m_server.start_accept();
	m_server.run();
}

int MultiplayerServer::findIndex(connection_hdl connection) {
	int playerIndex = 0;

	
	auto neededIteration = m_connections.find(connection);
	for (MultiplayerServer::ConnectionList::iterator currentConn = m_connections.begin(); currentConn != m_connections.end(); currentConn++) {
		if (neededIteration == currentConn) {

			// return the element
			return playerIndex;
			break;
		}
		playerIndex++;
	}
}

MultiplayerServer::MultiplayerServer() {
	
	m_server.init_asio();

	m_server.set_open_handler(CC_CALLBACK_1(MultiplayerServer::onServerOpen, this));
	m_server.set_close_handler(CC_CALLBACK_1(MultiplayerServer::onServerClose, this));
	m_server.set_message_handler(CC_CALLBACK_2(MultiplayerServer::onServerMessage, this));
}

void MultiplayerServer::start(unsigned short port) {
	boost::thread serverThread(CC_CALLBACK_1(MultiplayerServer::run, this), port);
}

// Sends a message to a player
void MultiplayerServer::sendMessage(int playerIndex, std::string message) {
	auto playerConnection = m_connections.begin();
	
	for (int i = 0; i < playerIndex; i++)
		playerConnection++;

	m_server.send(*playerConnection, message, websocketpp::frame::opcode::text);
}

void MultiplayerServer::broadcastMessage(std::string message) {
	for (auto it : m_connections)
		m_server.send(it, message, websocketpp::frame::opcode::text);
}

int MultiplayerServer::getConnectedPlayerCount() {
	return m_connections.size();
}
#endif