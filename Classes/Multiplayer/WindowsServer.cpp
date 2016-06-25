#include "CCPlatformMacros.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#include "WindowsServer.h"

#include "base/CCConsole.h"

#pragma comment(lib, "Ws2_32.lib")


/* I apoligize on behalf of microsoft for making such horrible type names */

void WindowsServer::initialize() {
	WSADATA winSocketData;

	int initializationResult = WSAStartup(MAKEWORD(2,2), &winSocketData);

	if (initializationResult != 0) {
		// ERROR
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	initializationResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (initializationResult != 0) {
		// ERROR
		WSACleanup();
	}

	listenSocket = INVALID_SOCKET;

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		// ERROR
		WSACleanup();
	}

	initializationResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (initializationResult == SOCKET_ERROR) {
		//ERROR
		WSACleanup();
	}
	freeaddrinfo(result);

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		// ERROR
		WSACleanup();
	}

	u_long nonBlock = 1;
	if (ioctlsocket(listenSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
	{
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
	}

}


void WindowsServer::update() {

	FD_SET fds;
	FD_ZERO(&fds);

	FD_SET(listenSocket, &fds);

	TIMEVAL tv;
	tv.tv_usec = 1000;

	if (select(listenSocket + 1, &fds, 0, 0, &tv) == 1) {
		SOCKET AcceptSocket;
		AcceptSocket = accept(listenSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET) {
			//ERROR

		}
		else {
			CCLOG("Client Accepted");
		}
	}
}

#endif