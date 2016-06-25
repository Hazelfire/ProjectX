#include "cocos2d.h"
#include "../UIScenes/PlayerSelector.h"

#define NOT_MOBILE (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)

// included in mobile for default port
#include "XServer.h"


#include "XClient.h"

class MultiplayerLobby : public cocos2d::Scene, private XClient::Lobby {
public:
	static MultiplayerLobby* create(GameInformation gameInfo, bool hostMode);
private:
	virtual bool init(GameInformation gameInfo, bool hostMode);

	void onSubmitAddress(std::string address);
#if NOT_MOBILE
	XServer* m_server;
#endif

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);

	// Lobby Connection Handlers
	virtual void onPlayerConnect(PlayerInformation playerInfo);
	virtual void onPlayerDisconnect(int playerIndex);
	virtual void onServerInformation(std::string hostName, std::string map);
	virtual void onError(std::string error);

	void updateClientList();
};