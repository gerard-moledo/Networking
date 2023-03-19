#include "Network.hpp"
#include "Lobby.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <cstdio>
#include <algorithm>

void Exit();

int main() {
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

				Packet data;
				data.id = packet->id;
				data.state = ConnectionState::lobby;
				Network::Send(data);
			}
		}
		if (packet && packet->state == ConnectionState::disconnected) {
			Network::clients.erase(std::remove_if(Network::clients.begin(), Network::clients.end(), 
												   [&](Client& client) { return client.id == packet->id; }), 
												  Network::clients.end());
		}

		Lobby::Update(packet);

		for (Game& game : Network::games) {
			bool shouldUpdateState = packet && packet->state == ConnectionState::game && game.IsAPlayer(packet->id);
			if (shouldUpdateState)
				game.Update(packet);
		}
	}

	exit(0);
}

void Exit() {
	Network::Deinitialize();
}