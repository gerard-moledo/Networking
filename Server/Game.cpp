#include "Game.hpp"

#include "Network.hpp"

void Player::Setup(uint64_t clientId, bool isFirst) {
	id = clientId;
	phase = isFirst ? Phase::start : Phase::wait;

	for (size_t i = 0; i < cards.size(); i++) {
		cards[i].id = i;
		cards[i].isSelected = false;
		cards[i].place = Place::deck;
		cards[i].type = (Type)(rand() % 4);
	}
}

void Player::UpdateState(Packet packet) {
	if (id == packet.id) {
		phase = packet.phase;

		for (size_t i = 0; i < cards.size(); i++) {
			cards[i] = packet.cards[i];
		}
	}
	if (id != packet.id && packet.phase == Phase::end) {
		phase = Phase::start;
	}
}

void Player::SendState() {
	Packet data{};
	data.id = id;
	data.state = ConnectionState::game;
	data.phase = phase;
	for (size_t i = 0; i < cards.size(); i++) {
		data.cards[i] = cards[i];
	}
	Network::sendBacklog.emplace_back(data);
}


Client::Client(SOCKET mSocket)
	: mSocket(mSocket), id(0), state(ConnectionState::lobby)
{

}


Game::Game(uint32_t id, Client clientHost, Client clientPeer)
	: id(id), host(clientHost), peer(clientPeer)
{
	host.player.Setup(clientHost.id, true);
	peer.player.Setup(clientPeer.id, false);
}

void Game::Update(Packet packet) {
	if (packet.id != 0) {
		host.player.UpdateState(packet);
		peer.player.UpdateState(packet);
	}

	host.player.SendState();
	peer.player.SendState();
}

bool Game::IsAPlayer(uint64_t id) {
	return id == host.id || id == peer.id;
}