#include "Game.hpp"

namespace Game {
	Player hostPlayer;
	Player connectedPlayer;
}

void Player::Setup(Client client, bool isFirst) {
	id = client.id;

	for (int i = 0; i < cards.size(); i++) {
		cards[i].id = i;
		cards[i].place = Place::deck;
		cards[i].type = (Type)(rand() % 4);
	}

	if (isFirst) phase = Phase::start;
	else		 phase = Phase::wait;
}

void Player::UpdateCards(std::vector<CardState> newStates) {
	cards = newStates;
}

Player& Game::GetPlayerByClientId(uint64_t id) {
	if (id == hostPlayer.id)
		return hostPlayer;
	else
		return connectedPlayer;
}

void Game::Setup() {
	hostPlayer.Setup(Network::clients[0], true);
	if (Network::clients.size() > 1) 
		connectedPlayer.Setup(Network::clients[1], false);
}

void Game::SendPlayerData(Player player) {
	Packet data;
	data.phase = player.phase;
	data.id = player.id;
	int cardsSize = (int)player.cards.size();
	for (int i = 0; i < cardsSize; i++) {
		data.cards[i] = player.cards[i];
	}
	data.status = ConnectionStatus::toGame;

	Network::Send(data);
}