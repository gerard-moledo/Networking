#include <stdio.h>
#include <chrono>

#include "Network.hpp"
#include "Graphics.hpp"
#include "Entity.hpp"

int main() {
	bool networkInitialized = InitializeNetwork();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
	}
	
	InitializeGraphics();

	Entity entity;
	Packet packetData;

	char data = 0;
	char dataLastFrame = 0;
	std::chrono::steady_clock::duration dPassed[1]{};
	std::chrono::steady_clock::time_point tPassed[2]{};
	std::chrono::steady_clock::time_point tPrev = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point tStacked = tPrev;
	float tInterpolate = 0.0f;
	float timestep = 0.01f;
	while (CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = tCurrent - tPrev;
		tPrev = tCurrent;
		tStacked = tStacked + dt;

		if (CheckInput('w', InputType::pressed)) data = data | (1 << 3);
		if (CheckInput('w', InputType::released)) data = data & ~(1 << 3);
		if (CheckInput('a', InputType::pressed)) data = data | (1 << 2);
		if (CheckInput('a', InputType::released)) data = data & ~(1 << 2);
		if (CheckInput('s', InputType::pressed)) data = data | (1 << 1);
		if (CheckInput('s', InputType::released)) data = data & ~(1 << 1);
		if (CheckInput('d', InputType::pressed)) data = data | (1 << 0);
		if (CheckInput('d', InputType::released)) data = data & ~(1 << 0);

		if (data != dataLastFrame) {
			dataLastFrame = data;
			SendData(data);
		}
		
		if (Listen()) {
			tPassed[1] = tPassed[0];
			tPassed[0] = std::chrono::steady_clock::now();
			dPassed[0] = tPassed[0] - tPassed[1];

			Receive(&packetData);

			entity.xPrev = entity.x;
			entity.yPrev = entity.y;
			entity.x = packetData.x;
			entity.y = packetData.y;
		}


		float tInterpolate = dPassed[0].count() == 0 ? 0 : (float)(tStacked - (tPassed[0])).count() / dPassed[0].count();
		
		BeginGraphics();
			entity.Render(tInterpolate);
		EndGraphics();
	}
	return 0;
}