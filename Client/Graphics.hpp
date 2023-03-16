#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Util.hpp"

enum struct InputType { pressed, held, released };

namespace Graphics {
	void Initialize();
	void Deinitialize();

	void RenderSession(uint64_t sessionId);
	void RenderWaiting();

	bool CheckWindowOpen();
	void MoveWindow(int x, int y);

	bool CheckKeyInput(char key, InputType type);

	void Begin();
	void End();
}
#endif