#include "Game.hpp"
#include "raylib.h"

#include <algorithm>

#define DECK_SLOT Rectangle{ 755 - 25, 550 - 35, 50, 70 }  
#define OPP_DECK_SLOT Rectangle{ 755 - 25, 600 - 550 - 35, 50, 70 }
#define TURN_BUTTON Rectangle { 720 - 60, 300 - 25, 120, 50 }
#define TINT(t) Color { 100, 100, 140, t }

namespace Game {
	Collection collection;
	Collection opponentCollection;

	Card* selectedCard = nullptr;
	bool queueMessage = false;
	
	Phase phase = Phase::wait;
	extern float tint = 0.0f;
}

void Game::HandleInput() {
	float mouseX = GetMousePosition().x;
	float mouseY = GetMousePosition().y;

	if (IsMouseButtonPressed(0)) {
		for (Card& card : collection.cards) {
			if (card.place == Place::deck) continue;

			if (card.CheckPointInBody(mouseX, mouseY)) {
				selectedCard = &card;
				selectedCard->SetSelection(true);
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
				else if (!selectedCard->CheckBodyOnField() && selectedCard->place == Place::field) {
					selectedCard->SetPlace(Place::hand);
				}
			}
			else {
				if (selectedCard->place == Place::hand && selectedCard->y < 500) {
					selectedCard->SetPosition(selectedCard->x, 500);
				}				
				if (selectedCard->place == Place::field && selectedCard->y > 500) {
					selectedCard->SetPosition(selectedCard->x, 500);
				}
			}

			collection.shouldModify = true;
		}
	}
	if (IsMouseButtonReleased(0)) {
		if (phase == Phase::play && CheckCollisionPointRec(Vector2{ mouseX, mouseY }, TURN_BUTTON)) {
			EndTurn();
		}
		if (selectedCard) {
			collection.shouldModify = true;

			selectedCard->isSelected = false;
			selectedCard = nullptr;

			queueMessage = true;
		}
	}
}

void Game::Update(float dt) {
	for (Card& card : collection.cards) {
		card.Update(dt);
	}

	for (Card& card : opponentCollection.cards) {
		card.Update(dt);
	}
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

	DrawRectangle(0, 500, 800, 100, TINT((unsigned char) tint));

	DrawRectangle(0, 0, 800, 100, TINT((unsigned char) (110.0f - tint)));

	for (Card& card : opponentCollection.cards) {
		card.Render();
	}

	for (Card& card : collection.cards) {
		if (card.isSelected) continue;
		card.Render();
	}
	if (selectedCard) 
		selectedCard->Render();
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
		if (index >= collection.deck.size()) break;

		if (collection.deck[index]->place == Place::deck) {
			collection.deck[index]->SetPlace(Place::hand);
			count++;
		}

		index++;
	}

	collection.shouldModify = true;
	queueMessage = true;
}

void Game::OrderCards(Collection& playerCollection) {
	if (playerCollection.shouldModify) {
		Order(playerCollection, Place::hand);
		Order(playerCollection, Place::field);

		playerCollection.shouldModify = false;
	}
}

void Game::Order(Collection& playerCollection, Place place) {
	std::vector<Card*>& container = *playerCollection.FillContainers(place);

	float gap = 0.0f, level = 0.0f;
	if (place == Place::hand) {
		gap = 52.0f; 
		level = playerCollection.id == collection.id ? 550.0f : 50.0f;
	};
	if (place == Place::field) {
		gap = 70.0f; 
		level = playerCollection.id == collection.id ? 400.0f : 200.0f; 
	};

	bool (*Sorter) (Card* first, Card* second);
	if (playerCollection.id == collection.id) {
		Sorter = [](Card* leftCard, Card* rightCard) { return leftCard->x < rightCard->x; };
	}
	else {
		Sorter = [](Card* first, Card* next) { return first->state.index < next->state.index; };
	}

	std::sort(container.begin(), container.end(), Sorter);

	for (int i = 0; i < container.size(); i++) {
		Card& card = *container[i];
		if (card.isSelected) continue;

		card.state.index = i;

		if (playerCollection.id != collection.id)
			if (place == Place::field) card.isFaceUp = true;
			else					  card.isFaceUp = false;

		float targetX = 400.0f + (i - (container.size() - 1) / 2.0f) * gap;
		card.SetTarget(targetX, level);
	}
}