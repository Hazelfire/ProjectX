#include "MultiplayerClient.h"
#include "base/CCConsole.h"


using namespace cocos2d::network;
void MultiplayerClient::connect(std::string ip, std::string port) {

	if (!m_socket.init(*this, "ws://" + ip + ":" + port, nullptr)) {
		CCLOG("FAILURE");
	}
	
}

MultiplayerClient::~MultiplayerClient() {
	m_socket.close();
}

void MultiplayerClient::sendMessage(std::string data) {
	m_socket.send(data);
}

void MultiplayerClient::onOpen(cocos2d::network::WebSocket* client) {
	onConnect();
}
void MultiplayerClient::onMessage(cocos2d::network::WebSocket* client, const cocos2d::network::WebSocket::Data& data) {
	std::string stringData = data.bytes;
	
	onMessage(stringData);
}
void MultiplayerClient::onClose(cocos2d::network::WebSocket* client) {
	onClose();
}
void MultiplayerClient::onError(cocos2d::network::WebSocket* client, const cocos2d::network::WebSocket::ErrorCode &code) {
	if (code == WebSocket::ErrorCode::CONNECTION_FAILURE) 
		onError("Connection Failed");
	else if (code == WebSocket::ErrorCode::TIME_OUT) 
		onError("Connection Timed Out");
	else if(code == WebSocket::ErrorCode::UNKNOWN) 
		onError("Unknown Error");
}
