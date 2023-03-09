#include "Graphics.hpp"

#include "raylib.h"

void InitializeGraphics() {
	InitWindow(1280, 720, "Game");
}

bool IsWindowOpen() {
	return !WindowShouldClose();
}

bool CheckInput(char key) {
	if (key == 'd') return IsKeyPressed(KEY_D);

	return false;
}

void DrawGraphics() {
	BeginDrawing();
		ClearBackground(WHITE);

	EndDrawing();
}