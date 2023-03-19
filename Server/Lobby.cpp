#include "Lobby.hpp"

#include <cstdio>

namespace Lobby{
	std::vector<Client> clients;
}

void Lobby::AddClient(uint64_t id) {
	Client newClient;
	newClient.id = id;
	newClient.address = Network::senderAddress;
	newClient.state = ConnectionState::lobby;
	Network::clients.emplace_back(newClient);
}

void Lobby::Update(Packet* packet) {
	Client* host = nullptr;
	Client* peer = nullptr;
	for (Client& client : Network::clients) {
		if (client.state == ConnectionState::lobby) {
			if (host && !peer) peer = &client;
			if (!host)		   host = &client;
		}

		if (host && peer) {
			Lobby::CreateGame(*host, *peer);
			host = nullptr;
			peer = nullptr;
		}
	}
}

void Lobby::CreateGame(Client& host, Client& peer) {
	host.state = ConnectionState::game;
	peer.state = ConnectionState::game;

	Game newGame = Game(host.id, peer.id);
	newGame.Update(nullptr);
	Network::games.emplace_back(newGame);
}