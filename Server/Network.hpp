#ifndef NETWORK_H
#define NETWORK_H

#include "Util.hpp"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <vector>

struct Packet
{
	uint64_t id;
	Phase phase;
	CardState cards[10];
};

struct Client
{
	uint64_t id;
	sockaddr_in addr;
};

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern std::vector<Client> clients;
	extern int senderAddrSize;
	extern char dataBuffer[256];
	extern fd_set readSet;

	bool Initialize();
	void Deinitialize();

	bool CheckClientExists(uint64_t id);
	void AddClient(Client client);

	bool Listen();
	Client Receive(Packet* packetData);
	void Send(Packet data);
};

#endif