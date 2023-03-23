#include "Lobby.hpp"

#include <cstdio>

namespace Lobby{
	std::vector<Client> clients;
}

void Lobby::AddClient(SOCKET clientSocket) {
	Client newClient(clientSocket);
	Network::clients.emplace_back(newClient);
}

void Lobby::HandleDisconnections(Packet packet) {
	if (packet.state == ConnectionState::disconnected) {
		auto itGame = std::remove_if(Network::games.begin(), Network::games.end(),
										[&](Game& game) { return game.IsAPlayer(packet.id); });

		if (itGame != Network::games.end()) {
			//Network::clients.erase(std::remove_if(Network::clients.begin(), Network::clients.end(),
			//									  [&](Client& client) { return itGame->IsAPlayer(client.id); }),
			//									  Network::clients.end());
			Network::games.erase(itGame);
		}
	}
}

void Lobby::Update(Packet packet) {
	Client* host = nullptr;
	Client* peer = nullptr;
	for (Client& client : Network::clients) {
		if (client.state == ConnectionState::lobby) {
			Network::Send(packet, client);
			if (host && !peer) peer = &client;
			if (!host)		   host = &client;
		}

		if (host && host->id != 0 && peer && peer->id != 0) {
			Lobby::CreateGame(*host, *peer);
			host = nullptr;
			peer = nullptr;
		}
	}
}

void Lobby::CreateGame(Client& host, Client& peer) {
	host.state = ConnectionState::game;
	peer.state = ConnectionState::game;
	
	uint32_t gameId = 0;
	for (int i = 0; i < 32; i++) {
		gameId += ((uint32_t) (rand() % 2)) << i;
	}
	Game newGame = Game(gameId, host, peer);
	newGame.Update(Packet{});
	Network::games.emplace_back(newGame);
}