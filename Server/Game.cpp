#include "Game.hpp"

namespace Game {
	Player hostPlayer;
	Player connectedPlayer;
}

void Player::UpdateCards(std::vector<CardState> newStates) {
	cards = newStates;
}

Player& Game::GetPlayerByClientId(uint64_t id) {
	if (id == hostPlayer.clientId)
		return hostPlayer;
	else
		return connectedPlayer;
}