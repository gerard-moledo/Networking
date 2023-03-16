#ifndef NETWORK_H
#define NETWORK_H

#include "Util.hpp"

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <vector>

enum struct ConnectionStatus : uint8_t { none, toServer, toClient, toGame };

struct Packet
{
	uint64_t id;
	ConnectionStatus status;

	Phase phase;
	CardState cards[10];
};

struct Client
{
	uint64_t id;
	ConnectionStatus status;

	sockaddr_in addr;
};

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern std::vector<Client> clients;
	extern sockaddr_in senderAddr;
	extern int senderAddrSize;
	extern char dataBuffer[256];
	extern fd_set readSet;

	bool Initialize();
	void Deinitialize();

	Client CheckForClient();
	bool AddOrRemoveClient(Client& client);

	bool CheckClientExists(uint64_t id);
	void AddClient(Client client);
	void RemoveClient(Client client);

	bool Listen();
	void Receive(Packet* packetData);
	void Send(Packet data);
};

#endif