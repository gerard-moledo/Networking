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

	// NETWORK LOOP
	bool isGameReady = false;
	while (!isGameReady) {
		if (Network::Listen()) {
			Packet dataReceived;
			Client client = Network::Receive(&dataReceived);

			if (!Network::CheckClientExists(client.id)) {
				Network::AddClient(client);

				if (Network::clients.size() == 2) {
					isGameReady = true;

					Game::Setup();

					
					sendto(Network::Socket, (char*)&Network::clients[0].id, 8, 0, (SOCKADDR*)&Network::clients[0].addr, sizeof(Network::clients[0].addr));
					sendto(Network::Socket, (char*)&Network::clients[0].id, 8, 0, (SOCKADDR*)&Network::clients[1].addr, sizeof(Network::clients[1].addr));
				}
			}
		}
	}
	
	Game::Setup();

	Game::SendPlayerData(Game::hostPlayer);
	Game::SendPlayerData(Game::connectedPlayer);

	// Game Loop vars
	auto tCurrent = std::chrono::high_resolution_clock::now();
	auto tPrev = tCurrent;
	Tick physicsTick = { 0.0f, 0.04f };
	Tick networkTick = { 0.0f, 0.4f };

	// GAME LOOP
	while (true) {
		tCurrent = std::chrono::high_resolution_clock::now();
		float dt = (tCurrent - tPrev).count() / (float) 1E9;
		tPrev = tCurrent;
		networkTick.acc += dt;

		if (Network::Listen()) {
			Packet dataReceived;
			Client client = Network::Receive(&dataReceived);
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