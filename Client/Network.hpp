#ifndef NETWORK_H
#define NETWORK_H

#include "Entity.hpp"
#include "Game.hpp"
#include "Util.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <queue>
#include <cstdint>

constexpr int BUFFER_SIZE = sizeof Packet * 200;

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;
	extern sockaddr_in serverAddr;
	extern int serverAddrSize;

	extern std::queue<Packet> packetQueue;
	extern fd_set readSet;
	extern fd_set writeSet;

	extern char dataBuffer[BUFFER_SIZE];

	extern uint64_t sessionId;
	extern ConnectionState state;

	bool Initialize();
	void Deinitialize();

	void SendState();
	void Send(Packet data);
	bool Listen(bool checkRead, bool checkWrite);
	void ReceivePackets();
}

#endif