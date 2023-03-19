#ifndef NETWORK_H
#define NETWORK_H

#include "Entity.hpp"
#include "Util.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <cstdint>

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern sockaddr_in serverAddr;
	extern int serverAddrSize;
	extern fd_set readSet;

	extern char data[256];

	extern uint64_t sessionId;
	extern ConnectionState state;

	bool Initialize();
	void Deinitialize();

	void Send(Packet data);
	bool Listen();
	Packet* Receive();
}

#endif