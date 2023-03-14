#ifndef GAME_H
#define GAME_H

#include "Entity.hpp"
#include "Graphics.hpp"

namespace Game {
	extern Collection collection;
	extern Collection opponentCollection;

	extern Card* selectedCard;

	extern bool queueMessage;

	extern Phase phase;
	
	extern float tint;

	void HandleInput();
	void Update(float dt);
	void Render(float dt);

	void StartTurn();
	void EndTurn();
	void DrawCard(int amount);

	void OrderCards(Collection& playerCollection);
	void Order(Collection& playerCollection, Place place);
}

#endif