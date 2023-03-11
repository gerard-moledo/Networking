#include "Entity.hpp"
#include "raylib.h"

#include <stdio.h>

Card::Card() {

}

Card::Card(float x, float y) : x(x), y(y) {

}

void Card::SetPosition(float xNew, float yNew) {
	x = xNew;
	y = yNew;
}

void Card::SetTarget(float tx, float ty) {
	isTargeting = true;
	targetX = tx;
	targetY = ty;
}

void Card::SetSelection(bool shouldSelect) {
	isSelected = shouldSelect;
}

void Card::SetPlace(Place placeNew) {
	place = placeNew;
	state.place = place;
}

bool Card::CheckPointInBody(float pointX, float pointY) {
	return CheckCollisionPointRec(Vector2{ pointX, pointY }, Rectangle{ x - width / 2, y - height / 2, width, height });
}
bool Card::CheckBodyOnField() {
	return x - width / 2> 0 && x + width / 2 < 800 && y - height / 2 > 0 && y + height / 2 < 500;
}

Place Card::Use() {
	Place placeNew;
	if (CheckBodyOnField()) {
		placeNew = Place::field;
	}
	else {
		placeNew = Place::hand;
	}

	SetPlace(placeNew);
	return place;
}

void Card::Update(float dt) {
	statePrev = state;

	if (isTargeting) {
		float xTween = x + (targetX - x) * 0.08f;
		float yTween = y + (targetY - y) * 0.08f;
		SetPosition(xTween, yTween);
		
		if ((targetX - x) * (targetX - x) < 1.0f &&
			(targetY - y) * (targetY - y) < 1.0f)
		{
			SetPosition(targetX, targetY);
			isTargeting = false;
			if (place == Place::deck) SetPlace(Place::hand);
		}
	}
}

void Card::Render(float t) {
	DrawRectangleV(Vector2{x - width / 2, y - height / 2}, Vector2{width, height}, BLACK);
}