#ifndef GAME_H
#define GAME_H

#include "Util.hpp"

#include <vector>

struct Player
{
	uint64_t id;

	bool isTurn;
	std::vector<CardState> cards = std::vector<CardState>(10);

	Player(uint64_t id);

	void UpdateState(Packet* packet);
	void SendState();
};

class Game {
public:
	Player host;
	Player peer;

	Game(uint64_t hostId, uint64_t peerId);

	void Update(Packet* packet);

	bool IsAPlayer(uint64_t id);
};

#endif