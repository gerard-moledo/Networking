#include "Network.hpp"
#include "Graphics.hpp"
#include "Game.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <vector>
#include <iostream>

void Setup(bool isFirst);
void EndProgram();

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

	bool isServerConnected = false;
	bool isClientConnected = false;
	bool isGameReady = false;

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

		Network::tLastReceived += dt.count() / (float)1E9;

		if (!isGameReady) {
			if (!isServerConnected) {
				isServerConnected = Network::CheckServerConnected();
			}
			else {
				if (!isClientConnected) {
					isClientConnected = Network::CheckClientConnected();
				}
				if (isClientConnected) {
					if (Network::Listen(0)) {
						Packet dataReceived;
						Network::Receive(&dataReceived);

						if (dataReceived.status < ConnectionStatus::toClient) 
							continue;

						Collection& playerCollection = dataReceived.id == Network::sessionId ? Game::collection : Game::opponentCollection;

						if (dataReceived.id == Network::sessionId) {
							Game::collection.id = dataReceived.id;
							Game::phase = dataReceived.phase;
						}
						else {
							Game::opponentCollection.id = dataReceived.id;
						}

						float xCardStart = 755;
						float yCardStart = dataReceived.id == Network::sessionId ? 550 : 50;

						for (CardState& state : dataReceived.cards) {

							Card card = Card(xCardStart, yCardStart, state.id, state.type, state.place);
							playerCollection.cards.emplace_back(card);
						}

						playerCollection.FillContainers();

						if (Game::opponentCollection.id != 0 && Game::collection.id != 0) {
							isGameReady = true;
						}

						Packet data;
						data.id = Network::sessionId;
						data.status = ConnectionStatus::toGame;
						data.phase = Game::phase;
						int cardsSize = (int)Game::collection.cards.size();
						for (int i = 0; i < cardsSize; i++) {
							data.cards[i] = Game::collection.cards[i].state;
						}
						Network::Send(data);

						if (isGameReady) {
							Game::DrawCard(5);
							if (Game::phase == Phase::start) {
								Graphics::MoveWindow(100, 250);
							}
							else {
								Graphics::MoveWindow(1000, 250);
							}

							Packet data;
							data.id = Network::sessionId;
							data.status = ConnectionStatus::toGame;
							data.phase = Game::phase;
							int cardsSize = (int)Game::collection.cards.size();
							for (int i = 0; i < cardsSize; i++) {
								data.cards[i] = Game::collection.cards[i].state;
							}
							Network::Send(data);
						}
					}
					else {
						Packet data;
						data.id = Network::sessionId;
						data.status = ConnectionStatus::toClient;
						Network::Send(data);
					}
				}
			}
			Graphics::Begin();
				Graphics::RenderSession(Network::sessionId);
				Graphics::RenderWaiting();
			Graphics::End();

			continue;
		}

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
			data.id = Network::sessionId;
			data.status = ConnectionStatus::toGame;
			data.phase = Game::phase;
			int cardsSize = (int) Game::collection.cards.size();
			for (int i = 0; i < cardsSize; i++) {
				data.cards[i] = Game::collection.cards[i].state;
			}
			Network::Send(data);
		}

		if (Network::Listen(0)) {
			Packet dataReceived;
			Network::Receive(&dataReceived);

			if (!dataReceived.id) {
				isGameReady = false;
				isClientConnected = false;
				Game::collection = Collection();
				Game::opponentCollection = Collection();
				continue;
			}

			if (dataReceived.status == ConnectionStatus::toGame) {
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

				Network::tLastReceived = 0.0f;
			}
		}

		Graphics::Begin();
			Game::Render(dt.count() / (float) 1E9);
			
			Graphics::RenderSession(Network::sessionId);
			if (Network::tLastReceived > 5.0f) Graphics::RenderWaiting();
		Graphics::End();
	}

	Graphics::Deinitialize();
	Network::Deinitialize();

	exit(0);
}

void EndProgram () {
	system("pause");
}