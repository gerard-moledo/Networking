#ifndef GAME_H
#define GAME_H

#include <vector>

enum struct Place { deck, hand, field };

struct CardState
{
	Place place;
};

struct Player
{
	uint64_t clientId;

	std::vector<CardState> cards;

	void UpdateCards(std::vector<CardState> cards);
};

namespace Game {
	extern Player hostPlayer;
	extern Player connectedPlayer;

	Player& GetPlayerByClientId(uint64_t id);
}

#endif