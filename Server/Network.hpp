#ifndef NETWORK_H
#define NETWORK_H

#include "Game.hpp"
#include "Util.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

constexpr int BUFFER_SIZE = 256;

struct Client {
	uint64_t id;
	sockaddr_in address;
	ConnectionState state;
};

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern sockaddr_in senderAddress;
	extern int senderAddressSize;
	extern char dataBuffer[BUFFER_SIZE];

	extern std::vector<Client> clients;
	extern std::vector<Game> games;

	bool Initialize();

	bool Listen();
	Packet* ReceivePacket();
	void Send(Packet data);

	void Deinitialize();
}

#endif