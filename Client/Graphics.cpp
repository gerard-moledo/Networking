#include "Graphics.hpp"

#include "raylib.h"

void InitializeGraphics() {
	InitWindow(800, 600, "Game");
}

bool CheckWindowOpen() {
	return !WindowShouldClose();
}

bool CheckInput(char key, InputType type) {
	bool (* InputFunc) (int);
	if (type == InputType::pressed)		  InputFunc = IsKeyPressed;
	else if (type == InputType::released) InputFunc = IsKeyReleased;
	else								  return false;

	if (key == 'w') return InputFunc(KEY_W);
	if (key == 'a') return InputFunc(KEY_A);
	if (key == 's') return InputFunc(KEY_S);
	if (key == 'd') return InputFunc(KEY_D);
	return false;
}

void BeginGraphics() {
	BeginDrawing();
	ClearBackground(RAYWHITE);
}

void EndGraphics() {
	EndDrawing();
}