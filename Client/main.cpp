#include "Network.hpp"
#include "Graphics.hpp"
#include "Game.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <vector>

int main () {
	srand(time(NULL));

	Graphics::Initialize();

	bool networkInitialized = Network::Initialize();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
	}
	
	long long tStart = time(NULL);
	int connectionResult = -1;
	while (connectionResult < 0 && Graphics::CheckWindowOpen()) {
		if (tStart != time(NULL)) {
			tStart == time(NULL);

			connectionResult = Network::WaitForClients();
		}

		Graphics::Begin();
		
		Graphics::End();
	}

	if (connectionResult <= 0) return connectionResult + 3;

	while (Game::opponentCollection.id == 0 || Game::collection.id == 0) {
		if (Network::Listen()) {
			Packet dataReceived;
			Network::Receive(&dataReceived);

			if (dataReceived.id == Network::sessionId) {
				Game::collection.id = dataReceived.id;
			}
			else {
				Game::opponentCollection.id = dataReceived.id;
			}

			float xCardStart = 755;
			float yCardStart = dataReceived.id == Game::collection.id ? 550 : 50;

			Collection& playerCollection = dataReceived.id == Game::collection.id ? Game::collection : Game::opponentCollection;

			for (CardState& state : dataReceived.cards) {
				Card newCard = Card(xCardStart, yCardStart, state.id, state.type, state.place);

				playerCollection.cards.emplace_back(newCard);
			}

			playerCollection.FillContainers();
		}
	}

	Game::DrawCard(5);
	if (connectionResult == 1) {
		Game::phase = Phase::start;
		Graphics::MoveWindow(100, 250);
	}
	else {
		Graphics::MoveWindow(1000, 250);
	}

	auto updateDuration = std::chrono::steady_clock::duration{};
	auto tPrev = std::chrono::steady_clock::now();
	auto tStacked = tPrev;
	std::chrono::steady_clock::time_point tPassed[2]{};
	float tInterpolate = 0.0f;
	while (Graphics::CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = tCurrent - tPrev;
		tPrev = tCurrent;
		tStacked = tStacked + dt;

		if (Game::phase == Phase::start) {
			Game::StartTurn();
		}

		Game::HandleInput();
		
		Game::OrderCards(Game::collection);
		Game::OrderCards(Game::opponentCollection);

		Game::Update(dt.count() / (float)1E9);

		if (Game::queueMessage) {
			Game::queueMessage = false;
			
			Packet data;
			data.phase = Game::phase;
			data.id = Network::sessionId;
			for (int i = 0; i < Game::collection.cards.size(); i++) {
				data.cards[i] = Game::collection.cards[i].state;
			}
			Network::SendData(data);
		}

		if (Network::Listen()) {
			Packet dataReceived;
			Network::Receive(&dataReceived);

			Collection& playerCollection = dataReceived.id == Game::collection.id ? Game::collection : Game::opponentCollection;

			for (CardState& state : dataReceived.cards) {
				auto itCard = std::find_if(playerCollection.cards.begin(), playerCollection.cards.end(), [&](Card& card) { return card.id == state.id; });
				if (itCard != playerCollection.cards.end()) 
					(*itCard).SetState(state);
			}

			if (dataReceived.id != Network::sessionId && dataReceived.phase == Phase::end) {
				Game::phase = Phase::start;
			}
			if (Game::phase == Phase::end) {
				Game::phase = Phase::wait;
			}
			playerCollection.shouldModify = true;
			playerCollection.FillContainers();
		}

		Graphics::Begin();
			Game::Render(dt.count() / (float) 1E9);
		Graphics::End();
	}

	return 0;
}