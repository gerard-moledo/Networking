#include "Entity.hpp"
#include "raylib.h"

void Entity::Render(float t) {
	float xInterpolated = xPrev * (1 - t) + x * t;
	float yInterpolated = yPrev * (1 - t) + y * t;
	DrawRectangleV(Vector2{ xInterpolated, yInterpolated }, Vector2{ 100.0f, 100.0f }, BLACK);
}