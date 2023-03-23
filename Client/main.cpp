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
void WaitForGame(Packet packet);
void RunGame(Packet packet, float dt); 

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
	Tick peerPing = { 0.0f, 10.0f };
	auto tPrev = std::chrono::steady_clock::now();
	while (Graphics::CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = (tCurrent - tPrev).count() / (float) 1E9;
		tPrev = tCurrent;
		serverPing.acc += dt;
		peerPing.acc += dt;

		if (serverPing.acc >= serverPing.step) {
			serverPing.acc = 0.0f;

			if (Network::state == ConnectionState::none) {
				int connectResult = connect(Network::Socket, (SOCKADDR*)&Network::serverAddr, sizeof Network::serverAddr);
				if (connectResult == SOCKET_ERROR) {
					printf("connect() failed or is unblocking (10035): %d\n", WSAGetLastError());
				}
			}
			if (Network::state == ConnectionState::lobby)
				PingServer();
		}
		
		if (Network::state == ConnectionState::none && Network::Listen(false, true)) {
			Network::state = ConnectionState::lobby;
		}
		if (Network::Listen(true, false)) {
			peerPing.acc = 0.0f;

			Network::ReceivePackets();
		}

		Packet packet{};
		if (!Network::packetQueue.empty()) {
			packet = Network::packetQueue.front();
			Network::packetQueue.pop();
		}

		switch (Network::state) {
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
			if (peerPing.acc >= peerPing.step)
				Graphics::RenderWaiting();
		Graphics::End();
	}

	Graphics::Deinitialize();
	Network::Deinitialize();

	exit(0);
}

void PingServer() {
	Packet data{};
	data.id = Network::sessionId;
	data.state = Network::state;
	data.phase = Game::phase;
	for (size_t i = 0; i < Game::host.cards.size(); i++) {
		data.cards[i] = Game::host.cards[i].state;
	}
	Network::Send(data);
}

void WaitForGame(Packet packet) {
	if (packet.state != ConnectionState::game)
		return;

	if (packet.id == Game::host.id || packet.id == Game::peer.id)
		return;

	bool isHost = packet.id == Network::sessionId;

	if (isHost) {
		Game::host.id = packet.id;
		Game::phase = packet.phase;
	}
	else Game::peer.id = packet.id;

	Player& player = isHost ? Game::host : Game::peer;

	for (CardState& state : packet.cards) {
		Card card = Card(isHost, state.id, state.type, state.place);
		player.cards.emplace_back(card);
	}

	if (Game::host.id != 0 && Game::peer.id != 0) {
		Network::state = ConnectionState::game;

		Game::Begin();
	}
}

void RunGame(Packet packet, float dt) {
	Game::Update(dt);

	if (Game::queueMessage) {
		Game::queueMessage = false;

		Network::SendState();
	}

	Game::UpdateState(packet);
}

void EndProgram () {
	Network::Deinitialize();
}