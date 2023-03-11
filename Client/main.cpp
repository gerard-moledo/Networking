#include "Network.hpp"
#include "Graphics.hpp"
#include "Entity.hpp"

#include <stdio.h>
#include <chrono>
#include <vector>
#include <algorithm>

struct Collection
{
	Card nullCard;
	std::vector<Card> deck = std::vector<Card>(10);
	std::vector<Card> hand{};
	std::vector<Card> field{};
	bool shouldModify = false;

	std::vector<Card*> MergeContainers(bool iterDeck, bool iterHand, bool iterField) {
		std::vector<Card*> multiContainer{};

		if (!iterDeck && !iterHand && !iterField) return multiContainer;

		if (iterDeck) 
			std::for_each(deck.begin(), deck.end(), [&](Card& card) { 
				multiContainer.emplace_back(&card); 
			});
		if (iterHand) 
			std::for_each(hand.begin(), hand.end(), [&](Card& card) { 
				multiContainer.emplace_back(&card); 
			});
		if (iterField) 
			std::for_each(field.begin(), field.end(), [&](Card& card) { 
				multiContainer.emplace_back(&card); 
			});

		return multiContainer;
	}
#define HAND_AND_FIELD(c) c.MergeContainers(false, true, true)
#define EVERYWHERE(c) c.MergeContainers(true, true, true)
};

void DrawCard(Collection& collection);
void PlayCard(Collection& collection, Card*& selectedCard);
void ReturnCardToHand(Collection& collection, Card*& selectedCard);

void OrderHand(Collection& collection, Card*& selectedCard);
void OrderField(Collection& collection, Card*& selectedCard);

int main() {
	srand(time(NULL));

	InitializeGraphics();

	bool networkInitialized = InitializeNetwork();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
	}

	struct
	{
		float x = 730;
		float y = 515;
		float width = 50;
		float height = 70;
	} deck;

	Collection collection;

	Card* selectedCard = nullptr;
	
	long long tStart = time(NULL);
	bool isConnected = false;
	while (!isConnected && CheckWindowOpen()) {
		if (tStart != time(NULL)) {
			tStart == time(NULL);

			if (Listen()) {
				Packet dataReceived;
				Receive(&dataReceived);

				if (dataReceived.state) {
					isConnected = true;
				}
			}
			else {
				SendData(Packet{ GetSessionId() });
			}
		}
		BeginGraphics();
		
		EndGraphics();
	}

	if (!isConnected) return 2;

	auto updateDuration = std::chrono::steady_clock::duration{};
	auto tPrev = std::chrono::steady_clock::now();
	auto tStacked = tPrev;
	std::chrono::steady_clock::time_point tPassed[2]{};
	float tInterpolate = 0.0f;
	while (CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = tCurrent - tPrev;
		tPrev = tCurrent;
		tStacked = tStacked + dt;

		float mouseX, mouseY;
		if (CheckMouseInput(0, InputType::pressed, &mouseX, &mouseY)) {
			for (Card*& card : HAND_AND_FIELD(collection)) {
				if (card->CheckPointInBody(mouseX, mouseY)) {
					selectedCard = &(*card);
					selectedCard->isSelected = true;
					selectedCard->shouldInterpolate = false;
					break;
				}
			}
		}
		if (CheckMouseInput(0, InputType::held, &mouseX, &mouseY)) {
			if (selectedCard) {
				selectedCard->SetPosition(mouseX, mouseY);

				collection.shouldModify = true;
				if (selectedCard->CheckBodyOnField() && selectedCard->place == Place::hand) {
					PlayCard(collection, selectedCard);
					selectedCard->place = Place::field;
				}
				else if (!selectedCard->CheckBodyOnField() && selectedCard->place == Place::field) {
					ReturnCardToHand(collection, selectedCard);
					selectedCard->place = Place::hand;
				}
			}
		}
		if (CheckMouseInput(0, InputType::released, &mouseX, &mouseY)) {
			if (CheckPointInRect(mouseX, mouseY, &deck)) {
				DrawCard(collection);
			}
			if (selectedCard) {
				collection.shouldModify = true;

				selectedCard->isSelected = false;
				selectedCard = nullptr;
			}
		}

		if (collection.shouldModify) {
			OrderHand(collection, selectedCard);
			OrderField(collection, selectedCard);

			collection.shouldModify = false;
		}

		for (Card*& card : EVERYWHERE(collection)) {
			card->Update(dt.count() / (float)1E9);
		}
		
		if (Listen()) {
			tPassed[1] = tPassed[0];
			tPassed[0] = std::chrono::steady_clock::now();
			updateDuration = tPassed[0] - tPassed[1];

			Packet dataReceived;
			Receive(&dataReceived);
		}


		float tInterpolate = updateDuration.count() == 0 ? 0 : (float)(tStacked - (tPassed[0])).count() / updateDuration.count();
		
		BeginGraphics();
			RenderDeck(&deck);

			for (Card*& card : HAND_AND_FIELD(collection)) {
				card->Render(tInterpolate);
			}
		EndGraphics();
	}
	return 0;
}

void DrawCard(Collection& collection) {
	if (collection.deck.size() <= 0) return;

	collection.hand.emplace_back(collection.deck[0]);
	collection.hand.back().place = Place::hand;
	collection.deck.erase(collection.deck.begin());

	collection.shouldModify = true;
}
void PlayCard(Collection& collection, Card*& selectedCard) {
	if (collection.hand.size() <= 0) return;

	auto& hand = collection.hand;
	auto& field = collection.field;

	auto itCard = std::find_if(hand.begin(), hand.end(), [](Card card) { return card.isSelected; });
	if (itCard == hand.end()) {
		printf("No card found to play.");
		return;
	}

	field.emplace_back(*itCard);
	hand.erase(itCard);

	selectedCard = &field.back();
}
void ReturnCardToHand(Collection& collection, Card*& selectedCard) {
	if (collection.field.size() <= 0) return;

	auto& hand = collection.hand;
	auto& field = collection.field;

	auto itCard = std::find_if(field.begin(), field.end(), [](Card card) { return card.isSelected; });
	if (itCard == field.end()) {
		printf("No card found to return to hand.");
		return;
	}

	hand.emplace_back(*itCard);
	field.erase(itCard);

	selectedCard = &hand.back();

	collection.shouldModify = true;
}

void OrderHand(Collection& collection, Card*& selectedCard) {
	std::sort(collection.hand.begin(), collection.hand.end(), [](Card leftCard, Card rightCard) {
		return leftCard.x < rightCard.x;
	});
	
	auto itCard = std::find_if(collection.hand.begin(), collection.hand.end(), [](Card card) {
		return card.isSelected;
	});
	if (itCard != collection.hand.end()) {
		selectedCard = &*itCard;
	}

	for (int i = 0; i < collection.hand.size(); i++) {
		if (collection.hand[i].isSelected) continue;

		float targetX = 400.0f + (i - (collection.hand.size() - 1) / 2.0f) * 60.0f;

		collection.hand[i].SetTarget(targetX, 550);
	}
}

void OrderField(Collection& collection, Card*& selectedCard) {
	std::sort(collection.field .begin(), collection.field.end(), [](Card leftCard, Card rightCard) {
		return leftCard.x < rightCard.x;
	});
	auto itCard = std::find_if(collection.field.begin(), collection.field.end(), [](Card card) {
		return card.isSelected;
	});
	if (itCard != collection.field.end()) {
		selectedCard = &*itCard;
	}

	for (int i = 0; i < collection.field.size(); i++) {
		if (collection.field[i].isSelected) continue;

		float targetX = 400.0f + (i - (collection.field.size() - 1) / 2.0f) * 80.0f;

		collection.field[i].SetTarget(targetX, 400);
	}
}