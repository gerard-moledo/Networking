#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Util.hpp"

enum struct InputType { pressed, held, released };

namespace Graphics {
	void Initialize();

	bool CheckWindowOpen();
	void MoveWindow(int x, int y);

	bool CheckKeyInput(char key, InputType type);

	void Begin();
	void End();
}
#endif