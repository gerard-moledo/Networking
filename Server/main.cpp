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
	int networkInitialized = Network::Initialize();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
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

					Packet data;
					data.state = 1;
					Network::Send(data);
				}
			}
		}
	}
	
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
		physicsTick.acc += dt;
		networkTick.acc += dt;

		if (Network::Listen()) {
			Packet dataReceived;
			Client client = Network::Receive(&dataReceived);

			Player& player = Game::GetPlayerByClientId(client.id);
			player.UpdateCards(dataReceived.cards);
		}

		if (isGameReady) {
			while (physicsTick.acc > physicsTick.step) {
				physicsTick.acc -= physicsTick.step;

			}
		}

		if (networkTick.acc > networkTick.step) {
			networkTick.acc = 0;

			for (Client& client : Network::clients) {
				Packet data{};
				data.id = client.id;
				data.cards = Game::GetPlayerByClientId(client.id).cards;
				Network::Send(data);
			}
		}
	}

	Network::Deinitialize();

	return 0;
}