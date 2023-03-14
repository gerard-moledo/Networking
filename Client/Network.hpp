#ifndef NETWORK_H
#define NETWORK_H

#include "Entity.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <cstdint>

struct Packet
{
	uint64_t id;
	Phase phase;
	CardState cards[10];
};

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern sockaddr_in serverAddr;
	extern int serverAddrSize;
	extern fd_set readSet;

	extern char data[256];

	extern uint64_t sessionId;

	bool Initialize();
	int WaitForClients();

	void SendData(Packet data);
	bool Listen();
	void Receive(Packet* packet);
}

#endif