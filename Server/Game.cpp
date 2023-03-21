#include "Game.hpp"

#include "Network.hpp"

Player::Player(uint64_t id) : id(id), isTurn(false) {
	for (size_t i = 0; i < cards.size(); i++) {
		cards[i].id = i;
		cards[i].isSelected = false;
		cards[i].place = Place::deck;
		cards[i].type = (Type)(rand() % 4);
	}
}

void Player::UpdateState(Packet* packet) {
	if (packet && id == packet->id) {
		isTurn = packet->isTurn;

		for (size_t i = 0; i < cards.size(); i++) {
			cards[i] = packet->cards[i];
		}
	}
	if (packet && id != packet->id && !packet->isTurn) {
		isTurn = true;
	}
}

void Player::SendState() {
	Packet data{};
	data.id = id;
	data.state = ConnectionState::game;
	data.isTurn = isTurn;
	for (size_t i = 0; i < cards.size(); i++) {
		data.cards[i] = cards[i];
	}
	Network::sendQueue.emplace_back(data);
}

Game::Game(uint32_t id, uint64_t hostId, uint64_t peerId) : id(id), host(Player(hostId)), peer(Player(peerId)) {
	host.isTurn = true;
}

void Game::Update(Packet* packet) {
	host.UpdateState(packet);
	peer.UpdateState(packet);

	host.SendState();
	peer.SendState();
}

bool Game::IsAPlayer(uint64_t id) {
	return id == host.id || id == peer.id;
}