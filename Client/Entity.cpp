#include "Graphics.hpp"

#include "Entity.hpp"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

constexpr float SPEED = 2;

Card::Card() {
	type = (Type) (rand() % 4);
	
	state.type = type;
}

Card::Card(uint16_t id, Type type, Place place) : id(id), type(type), place(place) {
	state.id = id;
	state.type = type;
	state.place = place;
}

Card::Card(float x, float y, uint16_t id, Type type, Place place) :
	x(x), y(y), id(id), type(type), place(place) 
{
	state.id = id;
	state.type = type;
	state.place = place;
}

void Card::SetPosition(float xNew, float yNew) {
	x = xNew;
	y = yNew;
}

void Card::SetTarget(float tx, float ty) {
	isTargeting = true;
	t = 0.0f;
	startX = x;
	startY = y;
	targetX = tx;
	targetY = ty;
}

void Card::SetSelection(bool shouldSelect) {
	isSelected = shouldSelect;
	isTargeting = false;
}

void Card::SetType(Type typeNew) {
	type = typeNew;
	state.type = type;
}

void Card::SetPlace(Place placeNew) {
	place = placeNew;
	state.place = place;
	isFaceUp = place != Place::deck;
}

void Card::SetState(CardState stateNew) {
	state = stateNew;
	
	id = state.id;
	type = state.type;
	place = state.place;
}

bool Card::CheckPointInBody(float pointX, float pointY) {
	return CheckCollisionPointRec(Vector2{ pointX, pointY }, Rectangle{ x - width / 2, y - height / 2, width, height });
}

bool Card::CheckBodyOnField() {
	return x - width / 2> 0 && x + width / 2 < 800 && y - height / 2 > 0 && y + height / 2 < 500;
}

void Card::Update(float dt) {
	if (isTargeting) {
		t += dt;

		float n = (1 - t * SPEED);
		float xTween = startX + (targetX - startX) * (1 - n * n * n * n * n);
		float yTween = startY + (targetY - startY) * (1 - n * n * n * n * n);
		SetPosition(xTween, yTween);
		
		if (t >= 1.0f / SPEED)
		{
			SetPosition(targetX, targetY);
			isTargeting = false;
		}
	}
}

void Card::Render() {
	Color color = GRAY;
	if (type == Type::white) color = WHITE;
	if (type == Type::black) color = DARKGRAY;
	if (type == Type::green) color = GREEN;
	if (type == Type::blue)  color = BLUE;
	if (!isFaceUp) color = DARKBROWN;

	DrawRectangleV(Vector2{ x - width / 2, y - height / 2 }, Vector2{ width, height }, color);
	DrawRectangleLinesEx(Rectangle{ x - width / 2, y - height / 2, width, height }, 2.0f, BLACK);
}