#ifndef NETWORK_H
#define NETWORK_H

#include "Entity.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <cstdint>

enum struct ConnectionStatus : uint8_t { none, toServer, toClient, toGame };

struct Packet
{
	uint64_t id;
	ConnectionStatus status;

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

	extern float tLastReceived;

	bool Initialize();
	void Deinitialize();

	bool CheckServerConnected();
	bool CheckClientConnected();

	void Send(Packet data);
	bool Listen(int waitTime);
	void Receive(Packet* packet);
}

#endif