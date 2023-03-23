#include "Game.hpp"
#include "Graphics.hpp"

#include "raylib.h"

#include <algorithm>

#define DECK_SLOT Rectangle{ 755 - 25, 550 - 35, 50, 70 }  
#define OPP_DECK_SLOT Rectangle{ 755 - 25, 600 - 550 - 35, 50, 70 }
#define TURN_BUTTON Rectangle { 720 - 60, 300 - 25, 120, 50 }
#define TINT(t) Color { 100, 100, 140, t }

namespace Game {
	Player host;
	Player peer;

	Card* selectedCard = nullptr;
	bool queueMessage = false;
	
	Phase phase = Phase::wait;
	extern float tint = 0.0f;
}

void Game::HandleInput() {
	float mouseX = GetMousePosition().x;
	float mouseY = GetMousePosition().y;

	Card* selectedCard = nullptr;
	auto itCard = std::find_if(host.cards.begin(), host.cards.end(), [&](Card& card) { return card.isSelected; });
	if (itCard != host.cards.end()) selectedCard = &*itCard;

	for (Card& card : host.cards) {
		if (card.place != Place::deck && !selectedCard) {
			if (card.CheckPointInBody(mouseX, mouseY)) {
				card.isHighlighted = true;
			}
			else {
				card.isHighlighted = false;
			}
		}
	}

	if (IsMouseButtonPressed(0)) {
		for (Card& card : host.cards) {
			if (card.place == Place::deck) continue;

			if (card.CheckPointInBody(mouseX, mouseY)) {
				selectedCard = &card;
				selectedCard->SetSelection(true);
				selectedCard->isTargeting = false;
				queueMessage = true;
				printf("PRESSED\n");
				break;
			}
		}
	}
	if (IsMouseButtonDown(0)) {
		if (selectedCard) {
			selectedCard->SetPosition(mouseX, mouseY);

			if (phase == Phase::play) {
				if (selectedCard->CheckBodyOnField() && selectedCard->place == Place::hand) {
					selectedCard->SetPlace(Place::field);
				}
				if (!selectedCard->CheckBodyOnField() && selectedCard->place == Place::field) {
					selectedCard->SetPlace(Place::hand);
				}
			}
			if (phase == Phase::wait) {
				if (selectedCard->place == Place::hand && selectedCard->y < 500) {
					selectedCard->SetPosition(selectedCard->x, 500);
				}				
				if (selectedCard->place == Place::field && selectedCard->y > 500) {
					selectedCard->SetPosition(selectedCard->x, 500);
				}
			}
			host.shouldOrderCards = true;
		}
	}
	if (IsMouseButtonReleased(0)) {
		if (phase == Phase::play && CheckCollisionPointRec(Vector2{ mouseX, mouseY }, TURN_BUTTON)) {
			EndTurn();
		}
		if (selectedCard) {
			host.shouldOrderCards = true;

			selectedCard->SetSelection(false);

			queueMessage = true;
			printf("RELEASED\n");
		}
	}
}

void Game::Begin() {
	DrawCard(5);

	if (Game::phase == Phase::start) Graphics::MoveWindow(100, 250);
	if (Game::phase == Phase::wait)  Graphics::MoveWindow(1000, 250);
}

void Game::StartTurn() {
	phase = Phase::draw;
	DrawCard(1);
	phase = Phase::play;
}

void Game::EndTurn() {
	phase = Phase::end;
	queueMessage = true;
}

void Game::DrawCard(int amount) {
	int count = 0;
	int index = 0;
	while (count < amount) {
		if (index >= host.cards.size()) break;

		if (host.cards[index].place == Place::deck) {
			host.cards[index].SetPlace(Place::hand);
			count++;
		}

		index++;
	}

	host.shouldOrderCards = true;
	queueMessage = true;
}

void Game::Update(float dt) {
	if (phase == Phase::start) {
		StartTurn();
	}

	HandleInput();

	OrderCards(host);
	OrderCards(peer);

	for (Card& card : host.cards) {
		card.Update(dt);
	}

	for (Card& card : peer.cards) {
		card.Update(dt);
	}
}

void Game::UpdateState(Packet packet) {
	if (packet.id == 0) return;

	if (phase == Phase::end)
		phase = Phase::wait;

	if (packet.id == peer.id && packet.phase == Phase::end && phase == Phase::wait)
		phase = Phase::start;

	Player* player = nullptr;
	if (packet.id == host.id) player = &host;
	if (packet.id == peer.id) 
		player = &peer;
	if (!player) return;

	for (CardState& state : packet.cards) {
		auto itCard = std::find_if(player->cards.begin(), player->cards.end(), [&](Card& card) { return card.id == state.id; });
		if (itCard != player->cards.end()) {
			itCard->SetState(state);


			if (player->id == peer.id) {
				itCard->state.index = state.index;
				itCard->isFaceUp = state.place == Place::field &&  !state.isSelected;
			}
		}
	}

	if (player->id == peer.id) 
		player->shouldOrderCards = true;
}

void Game::Render(float dt) {
	DrawRectangleRec(DECK_SLOT, DARKBROWN);
	DrawRectangleRec(OPP_DECK_SLOT, DARKBROWN);

	DrawRectangleRec(TURN_BUTTON, GOLD);
	DrawRectangleLines(720 - 60, 300 - 25, 120, 50, BLACK);
	if (phase == Phase::wait) {
		DrawText("WAITING", 730 - 60 + 6, 300 - 10, 20, BLACK);
	}
	if (phase != Phase::wait) {
		DrawText("END TURN", 722 - 60 + 6, 300 - 10, 20, BLACK);
	}

	if (phase == Phase::wait) DrawRectangleRec(TURN_BUTTON, Color{ 200, 200, 200, 120 });

	if (phase == Phase::wait) tint += dt * 110 * 5;
	else					  tint -= dt * 110 * 5;

	if (tint > 110.0f) tint = 110.0f;
	if (tint < 0.0f) tint = 0.0f;

	DrawRectangle(0, 500, 800, 100, TINT((unsigned char)tint));

	DrawRectangle(0, 0, 800, 100, TINT((unsigned char)(110.0f - tint)));

	auto RenderSorter = [](Card& bottom, Card& top) {
		return !(bottom.isSelected || bottom.isHighlighted)
			&& (top.isSelected || top.isHighlighted);
	};
	std::sort(peer.cards.begin(), peer.cards.end(), RenderSorter);
	for (Card& card : peer.cards) {
		card.Render();
	}
	std::sort(host.cards.begin(), host.cards.end(), RenderSorter);
	for (Card& card : host.cards) {
		card.Render();
	}
}

void Game::OrderCards(Player& player) {
	if (player.shouldOrderCards) {
		player.shouldOrderCards = false;

		Order(player, Place::hand);
		Order(player, Place::field);
	}
}

void Game::Order(Player& player, Place place) {
	float gap = 0.0f, level = 0.0f;
	if (place == Place::hand) {
		gap = 52.0f; 
		level = player.id == host.id ? 550.0f : 50.0f;
	};
	if (place == Place::field) {
		gap = 70.0f; 
		level = player.id == host.id ? 400.0f : 200.0f; 
	};

	std::vector<Card*> container = player.FillContainers(place);
	bool (*Sorter) (Card* first, Card* second);
	if (player.id == host.id) {
		Sorter = [](Card* leftCard, Card* rightCard) { return leftCard->x < rightCard->x; };
	}
	else {
		Sorter = [](Card* first, Card* next) { return first->state.index < next->state.index; };
	}

	std::sort(container.begin(), container.end(), Sorter);

	if (player.id == host.id) printf("Host:\n");
	for (size_t i = 0; i < container.size(); i++) {
		Card& card = *container[i];

		if (player.id == host.id && card.state.index != i) {
			queueMessage = true;
		}

		if (player.id == host.id) 
			printf("i: %llu, id: %u, index: %d\n\n", i, card.state.id, card.state.index);
		card.state.index = i;

		if (player.id == host.id && card.isSelected) continue;

		float targetX = 400.0f + (i - (container.size() - 1) / 2.0f) * gap;
		card.SetTarget(targetX, level);
	}
	printf("\n\n");
}