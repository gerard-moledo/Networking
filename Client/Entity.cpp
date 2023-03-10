#include "Entity.hpp"
#include "raylib.h"

#include <stdio.h>

Entity::Entity(float x, float y) : x(x), y(y)
{
	state.x = x;
	state.y = y;
	statePrev = state;
}

void Entity::SetPosition(float xNew, float yNew) {
	x = xNew;
	y = yNew;

	state.x = x;
	state.y = y;
}

bool Entity::CheckPointInBody(float pointX, float pointY) {
	return CheckCollisionPointRec(Vector2{ pointX, pointY }, Rectangle{ x - width / 2, y - height / 2, width, height });
}
bool Entity::CheckBodyOnField() {
	return x - width / 2> 0 && x + width / 2 < 800 && y - height / 2 > 0 && y + height / 2 < 500;
}

void Entity::SetTarget(float tx, float ty) {
	isTargeting = true;
	targetX = tx;
	targetY = ty;
	tTarget = 0.0f;
}

void Entity::PlaceOnField(float* fieldX, float* fieldY) {
	place = Place::field;

	SetTarget(400, 400);
}

void Entity::Update(float dt) {
	if (isTargeting) {
		float xTween = x + (targetX - x) * 0.08f;
		float yTween = y + (targetY - y) * 0.08f;
		SetPosition(xTween, yTween);
		
		if ((targetX - x) * (targetX - x) < 1.0f &&
			(targetY - y) * (targetY - y) < 1.0f)
		{
			SetPosition(targetX, targetY);
			isTargeting = false;
			if (place == Place::deck) place = Place::hand;
		}
	}
}

void Entity::Render(float t) {
	EntityState iState = Interpolate(t);
	Vector2 position{ iState.x - width / 2, iState.y - height / 2 };
	DrawRectangleV(position, Vector2{ width, height }, BLACK);
}

EntityState Entity::Interpolate(float t) {
	EntityState iState;
	iState.x = statePrev.x * (1 - t) + state.x * t;
	iState.y = statePrev.y * (1 - t) + state.y * t;

	return iState;
}