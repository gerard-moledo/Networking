#ifndef GAME_H
#define GAME_H

#include "Network.hpp"
#include "Util.hpp"

#include <vector>

struct Player
{
	uint64_t id;

	Phase phase;

	std::vector<CardState> cards = std::vector<CardState>(10);

	void Setup(Client client, bool isFirst);
	void UpdateCards(std::vector<CardState> cards);
};

namespace Game {
	extern Player hostPlayer;
	extern Player connectedPlayer;

	Player& GetPlayerByClientId(uint64_t id);

	void Setup();
	void SendPlayerData(Player player);
}

#endif