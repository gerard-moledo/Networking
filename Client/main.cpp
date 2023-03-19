#include "Network.hpp"
#include "Graphics.hpp"
#include "Game.hpp"
#include "Util.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <vector>
#include <iostream>

void EndProgram();

void PingServer();
void WaitForGame(Packet* packet);
void RunGame(Packet* packet, float dt); 


int main () {
	srand(time(NULL));
	atexit(EndProgram);

	bool networkInitialized = Network::Initialize();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		exit(1);
	}

	Graphics::Initialize();

	printf("%llu\n", Network::sessionId);

	Tick serverPing = { 0.0f, 1.0f };
	Tick peerPing = { 0.0f, 5.0f };
	auto tPrev = std::chrono::steady_clock::now();
	while (Graphics::CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = (tCurrent - tPrev).count() / (float) 1E9;
		tPrev = tCurrent;
		serverPing.acc += dt;
		peerPing.acc += dt;

		Packet* packet = nullptr;
		if (Network::Listen()) {
			packet = Network::Receive();
			if (packet) peerPing.acc = 0.0f;
		}

		switch (Network::state) {
			case ConnectionState::none:
				if (serverPing.acc >= serverPing.step) {
					serverPing.acc = 0;

					PingServer();
				}
				if (packet && packet->id == Network::sessionId)
					Network::state = ConnectionState::lobby;
				break;
			case ConnectionState::lobby: 
				WaitForGame(packet); 
				break;
			case ConnectionState::game:
				RunGame(packet, dt);
				break;
		}


		Graphics::Begin();
			Game::Render(dt);
			
			Graphics::RenderSession(Network::sessionId);
			if (peerPing.acc >= peerPing.step) Graphics::RenderWaiting();
		Graphics::End();
	}

	Graphics::Deinitialize();
	Network::Deinitialize();

	exit(0);
}

void PingServer() {
	Packet data;
	data.id = Network::sessionId;
	data.state = ConnectionState::none;
	Network::Send(data);
}

void WaitForGame(Packet* packet) {
	if (packet && packet->state == ConnectionState::game && Network::state != ConnectionState::game) {
		Collection& playerCollection = packet->id == Network::sessionId ? Game::collection : Game::opponentCollection;

		bool isPlayer = packet->id == Network::sessionId;

		if (isPlayer) Game::collection.id = packet->id;
		else		  Game::opponentCollection.id = packet->id;

		for (CardState& state : packet->cards) {
			Card card = Card(isPlayer, state.id, state.type, state.place);
			playerCollection.cards.emplace_back(card);
		}
		playerCollection.FillContainers();

		if (Game::collection.id != 0 && Game::opponentCollection.id != 0) {
			Network::state = ConnectionState::game;

			Game::Begin(packet);
		}
	}
}

void RunGame(Packet* packet, float dt) {
	Game::Update(dt);

	if (Game::queueMessage) {
		Game::queueMessage = false;

		Packet data;
		data.id = Network::sessionId;
		data.state = ConnectionState::game;
		data.isTurn = Game::phase != Phase::wait;
		for (size_t i = 0; i < Game::collection.cards.size(); i++) {
			data.cards[i] = Game::collection.cards[i].state;
		}
		Network::Send(data);
	}

	if (!packet || (packet->id != Game::collection.id && packet->id != Game::opponentCollection.id)) return;

	Game::UpdateState(packet);
}

void EndProgram () {

}