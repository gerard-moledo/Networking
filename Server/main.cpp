#include "Network.hpp"
#include "Game.hpp"

#include <stdio.h>
#include <iostream>
#include <bitset>
#include <chrono>
#include <vector>

struct Tick
{
	float acc;
	float step;
};

int main() {
	srand(time(NULL));

	int networkInitialized = Network::Initialize();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
	}

	printf("Network initialized.\n");

	// Game Loop vars
	auto tCurrent = std::chrono::high_resolution_clock::now();
	auto tPrev = tCurrent;
	Tick physicsTick = { 0.0f, 0.04f };
	Tick networkTick = { 0.0f, 0.4f };

	// GAME LOOP
	bool isGameReady = false;
	while (true) {
		tCurrent = std::chrono::high_resolution_clock::now();
		float dt = (tCurrent - tPrev).count() / (float)1E9;
		tPrev = tCurrent;
		networkTick.acc += dt;

		// NETWORK LOOP
		if (!isGameReady) {
			if (Network::Listen()) {
				Packet dataReceived;
				Network::Receive(&dataReceived);
				Client client = Client{ dataReceived.id, dataReceived.status, Network::senderAddr };
				if (client.status == ConnectionStatus::none) {
					Network::RemoveClient(client);
					if (Network::clients.size() > 0) Game::hostPlayer.Setup(Network::clients[0], true);
					isGameReady = false;
					Network::Send(Packet{ 0 });
				}
			}

			if (Network::clients.size() < 2) {
				Client newClient = Network::CheckForClient();
				if (!newClient.id) continue;

				Network::AddOrRemoveClient(newClient);
				for (Client& client : Network::clients) {
					Network::Send(Packet{ client.id, ConnectionStatus::toServer});
				}
				if (Network::clients.size() >= 2) {
					Game::Setup();
				}
			}
			else {
				if (Network::Listen()) {
					Packet dataReceived;
					Network::Receive(&dataReceived);

					if (dataReceived.status == ConnectionStatus::toGame) {
						isGameReady = true;
					}
					else {
						Game::SendPlayerData(Game::hostPlayer);
						Game::SendPlayerData(Game::connectedPlayer);
					}
				}
			}
			continue;
		}

		if (Network::Listen()) {
			Packet dataReceived;
			Network::Receive(&dataReceived);
			Client client = Client{ dataReceived.id, dataReceived.status, Network::senderAddr };
			if (client.status == ConnectionStatus::none) {
				Network::RemoveClient(client);
				if (Network::clients.size() > 0) Game::hostPlayer.Setup(Network::clients[0], true);
				isGameReady = false;
				Network::Send(Packet{ 0 });
				continue;
			}

			Player& player = Game::GetPlayerByClientId(client.id);
			player.phase = dataReceived.phase;
			for (int i = 0; i < 10; i++) {
				player.cards[i] = dataReceived.cards[i];
			}
			
			Game::SendPlayerData(player);
		}
	}

	Network::Deinitialize();

	return 0;
}