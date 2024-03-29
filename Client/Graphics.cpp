#include "Graphics.hpp"

#include "raylib.h"
#include <stdio.h>

#include <string>

void Graphics::Initialize() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Game");
}

void Graphics::RenderSession(uint64_t sessionId) {
	DrawText(std::to_string(sessionId).c_str(), 10, 10, 20, BLACK);
}
void Graphics::RenderWaiting() {
	DrawText("Connecting...", 105, 250, 100, WHITE);
}

bool Graphics::CheckWindowOpen() {
	return !WindowShouldClose();
}

void Graphics::MoveWindow(int x, int y) {
	SetWindowPosition(x, y);
	SetWindowState(FLAG_WINDOW_TOPMOST);
}


bool Graphics::CheckKeyInput(char key, InputType type) {
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

void Graphics::Begin() {
	BeginDrawing();
	ClearBackground(BEIGE);
	DrawFPS(10, 110);

	DrawRectangle(0, 0, 800, 100, BROWN);
	DrawRectangle(0, 500, 800, 100, BROWN);
}

void Graphics::End() {
	EndDrawing();
}

void Graphics::Deinitialize() {
	CloseWindow();
}