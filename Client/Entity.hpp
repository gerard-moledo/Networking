#ifndef ENTITY_H
#define ENTITY_H

#include "Util.hpp"

#include <cstdint>
#include <vector>

class Card
{
public:
	uint16_t id;

	float x = 400.0f;
	float y = 300.0f;
	float width = 50;
	float height = 70;

	bool isTargeting = false;
	float t = 0.0f;
	float startX = 0.0f;
	float startY = 0.0f;
	float targetX = 0.0f;
	float targetY = 0.0f;
	bool shouldInterpolate = true;

	Place place;
	Type type;

	bool isSelected = false;
	bool isFaceUp = false;
	
	CardState state{ };

	Card();
	Card(uint16_t id, Type type, Place place);
	Card(bool isHost, uint16_t id, Type type, Place place);

	void SetPosition(float x, float y);
	void SetTarget(float x, float y);
	void SetSelection(bool shouldSelect);
	void SetState(CardState stateNew);
	void SetType(Type type);
	void SetPlace(Place place);

	bool CheckPointInBody(float x, float y);
	bool CheckBodyOnField();

	void Update(float dt);
	void Render();
};

struct Collection
{
	uint64_t id;

	std::vector<Card> cards;

	std::vector<Card*> deck{};
	std::vector<Card*> hand{};
	std::vector<Card*> field{};
	bool shouldModify = false;

	void FillContainers() {
		deck.clear();
		hand.clear();
		field.clear();

		for (Card& card : cards) {
			if (card.place == Place::deck) deck.emplace_back(&card);
			if (card.place == Place::hand) hand.emplace_back(&card);
			if (card.place == Place::field) field.emplace_back(&card);
		}
	}

	std::vector<Card*>* FillContainers(Place place) {
		FillContainers();

		std::vector<Card*>* container = nullptr;
		if (place == Place::deck)  container = &deck;
		if (place == Place::hand)  container = &hand;
		if (place == Place::field) container = &field;

		return container;
	}
};

#endif