#include "MultiplayerServer.h"
#include "CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)

#include "GameInformation.h"

class XServer : public MultiplayerServer {
public:
	
	void init(std::string nameHost, std::string map);
protected:

	// Event handlers
	virtual void onOpen(int player);
	virtual void onClose(int player);
	virtual void onMessage(int player, std::string message);

private:

	// Information stored about the server
	struct StoredServerInfo {
		std::string nameHost;
		std::string map;
	};

	// Hold if it is in-game or not
	bool m_isGameState = false;

	// stored information on players
	std::vector<PlayerInformation> m_playerInfo;

	StoredServerInfo m_serverInfo;
};

#endif