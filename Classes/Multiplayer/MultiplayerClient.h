#pragma once
#include "network/WebSocket.h"

class MultiplayerClient : private cocos2d::network::WebSocket::Delegate{
public:
	void connect(std::string ip, std::string port);
	~MultiplayerClient();
private:
	virtual void onOpen(cocos2d::network::WebSocket* client) override;
	virtual void onMessage(cocos2d::network::WebSocket* client, const cocos2d::network::WebSocket::Data& data) override;
	virtual void onClose(cocos2d::network::WebSocket* client) override;
	virtual void onError(cocos2d::network::WebSocket* client, const cocos2d::network::WebSocket::ErrorCode& error) override;
	
	// Socket instance
	cocos2d::network::WebSocket m_socket;

protected:
	void sendMessage(std::string data);
	virtual void onConnect() = 0;
	virtual void onMessage(std::string& data) = 0;
	virtual void onClose() = 0;
	virtual void onError(std::string error) = 0;
};