#ifndef GRAPHICS_H
#define GRAPHICS_H

enum struct InputType { pressed, held, released };

void InitializeGraphics();

bool CheckWindowOpen();
bool CheckKeyInput(char key, InputType type);
bool CheckMouseInput(int button, InputType type, float* mouseX, float* mouseY);

bool CheckPointInRect(float x, float y, void* rect);

void RenderDeck(void* deck);

void BeginGraphics();
void EndGraphics();

#endif