#include "CCPlatformMacros.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>

#define DEFAULT_PORT "44075"

class WindowsServer {
public:
	void initialize();
	void update();
private:
	SOCKET listenSocket;
};
#endif