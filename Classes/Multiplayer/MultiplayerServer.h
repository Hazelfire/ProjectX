#include <cocos2d.h>
#include <string>
#define DEFAULT_PORT "44075"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <set>

using websocketpp::connection_hdl;

class MultiplayerServer {
public:

	// Informationation requires to know about the server
	struct ServerInformation {
		std::string mapName;
		std::string hostName;
	};

	MultiplayerServer();

	typedef websocketpp::server<websocketpp::config::asio> Server;
	
	void start(unsigned short port);

private:
	int findIndex(connection_hdl connection);

	// server events
	void onServerOpen(connection_hdl hdl);
	void onServerClose(connection_hdl hdl);
	void onServerMessage(connection_hdl hdl, Server::message_ptr msg);

	void run(unsigned short port);

	Server m_server;


	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> ConnectionList;

	ConnectionList m_connections;

protected:

	// Child functions
	virtual void onOpen(int player) = 0;
	virtual void onClose(int player) = 0;
	virtual void onMessage(int player, std::string message) = 0;

	void sendMessage(int player, std::string message);

	void broadcastMessage(std::string message);

	int getConnectedPlayerCount();
};

#endif
