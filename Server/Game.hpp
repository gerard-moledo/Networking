#ifndef GAME_H
#define GAME_H

#include "Util.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

struct Player
{
	uint64_t id;

	Phase phase;
	std::vector<CardState> cards = std::vector<CardState>(10);

	void Setup(uint64_t clientId, bool isFirst);

	void UpdateState(Packet packet);
	void SendState();
};

class Client {
public:
	SOCKET mSocket;

	uint64_t id;
	ConnectionState state;

	Player player;

	Client(SOCKET mSocket);
};

class Game {
public:
	uint32_t id;

	Client host;
	Client peer;

	Game(uint32_t id, Client clientHost, Client clientPeer);

	void Update(Packet packet);

	bool IsAPlayer(uint64_t id);
};

#endif