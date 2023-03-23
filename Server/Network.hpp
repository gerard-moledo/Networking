#ifndef NETWORK_H
#define NETWORK_H

#include "Game.hpp"
#include "Util.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <queue>

constexpr int BUFFER_SIZE = sizeof Packet * 200;

namespace Network {
	extern WSADATA wsaData;
	extern SOCKET Socket;

	extern sockaddr_in senderAddress;
	extern int senderAddressSize;
	extern char dataBuffer[BUFFER_SIZE];

	extern std::vector<Client> clients;
	extern std::vector<Game> games;

	extern std::queue<Packet> receiveQueue;
	extern std::vector<Packet> sendBacklog;

	bool Initialize();

	bool Listen(SOCKET socket);
	int ReceivePackets(SOCKET receiveSocket);
	void SendToGame(Packet data, Game game);
	void Send(Packet data, Client client);

	void Deinitialize();
}

#endif