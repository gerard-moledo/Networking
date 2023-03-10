#include "Graphics.hpp"

#include "raylib.h"
#include <stdio.h>

void InitializeGraphics() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Game");
}

bool CheckWindowOpen() {
	return !WindowShouldClose();
}

bool CheckKeyInput(char key, InputType type) {
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

bool CheckMouseInput(int button, InputType type, float* mouseX, float* mouseY) {
	if (mouseX) *mouseX = GetMousePosition().x;
	if (mouseY) *mouseY = GetMousePosition().y;

	if (type == InputType::pressed) return IsMouseButtonPressed(button);
	if (type == InputType::held) return IsMouseButtonDown(button);
	if (type == InputType::released) return IsMouseButtonReleased(button);
	
	return false;
}

void BeginGraphics() {
	BeginDrawing();
	ClearBackground(RAYWHITE);

	DrawRectangle(0, 500, 800, 100, SKYBLUE);
}

void EndGraphics() {
	EndDrawing();
}