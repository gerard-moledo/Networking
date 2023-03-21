#include "Network.hpp"
#include "Lobby.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <algorithm>

void Exit();

int main() {
	srand(time(NULL));
	atexit(Exit);

	bool isNetworkInitialized = Network::Initialize();
	if (!isNetworkInitialized) {
		printf("Network initialization failed.");
		exit(1);
	}

	while (true) {
		Packet* packet = nullptr;
		if (Network::Listen()) {
			packet = Network::ReceivePacket();
		}
		
		if (packet && packet->state == ConnectionState::none) {
			auto itFoundClient = std::find_if(Network::clients.begin(), Network::clients.end(), 
												[&](Client& client) { return client.id == packet->id; });
			if (itFoundClient == Network::clients.end()) {
				Lobby::AddClient(packet->id);

				Packet data{};
				data.id = packet->id;
				data.state = ConnectionState::lobby;
				Network::Send(data, Network::clients.back());
			}
		}
		if (packet && packet->state == ConnectionState::disconnected) {
			auto itClient = std::remove_if(Network::clients.begin(), Network::clients.end(),
														[&](Client& client) { return client.id == packet->id; });
			auto itGame = std::remove_if(Network::games.begin(), Network::games.end(),
											[&](Game& game) { return game.IsAPlayer(itClient->id); });
			Network::clients.erase(itClient, Network::clients.end());
			Network::games.erase(itGame, Network::games.end());
		}

		Lobby::Update(packet);

		for (Game& game : Network::games) {
			bool shouldUpdateState = packet && packet->state == ConnectionState::game && game.IsAPlayer(packet->id);
			if (shouldUpdateState) {
				game.Update(packet);
			}
			for (auto it = Network::sendQueue.begin(); it != Network::sendQueue.end();) {
				if (game.IsAPlayer(it->id)) {
					Network::SendToGame(*it, game);
					it = Network::sendQueue.erase(it);
					continue;
				}
				it++;
			}
		}


	}

	exit(0);
}

void Exit() {
	Network::Deinitialize();
}