#ifndef GAME_H
#define GAME_H

#include "Entity.hpp"

namespace Game {
	extern Player host;
	extern Player peer;

	extern bool queueMessage;

	extern Phase phase;
	
	extern float tint;

	void Begin();

	void HandleInput();
	void Update(float dt);
	void UpdateState(Packet packet);
	void Render(float dt);

	void StartTurn();
	void EndTurn();
	void DrawCard(int amount);

	void OrderCards(Player& player);
	void Order(Player& player, Place place);
}

#endif