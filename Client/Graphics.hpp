#ifndef GRAPHICS_H
#define GRAPHICS_H

enum struct InputType { pressed, held, released };

void InitializeGraphics();

bool CheckWindowOpen();
bool CheckInput(char key, InputType type);

void BeginGraphics();
void EndGraphics();

#endif