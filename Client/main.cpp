#include <stdio.h>

#include "Network.hpp"
#include "Graphics.hpp"


int main() {
	bool networkInitialized = InitializeNetwork();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
	}
	
	InitializeGraphics();
	while (IsWindowOpen()) {
		if (CheckInput('d')) {
			SendData((void *) "Hello");
		}

		DrawGraphics();
	}
	return 0;
}