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

	Entity entity = Entity(600, 400);
	Packet packetData;

	Packet data;
	Packet dataLastFrame;

	auto updateDuration = std::chrono::steady_clock::duration{};
	auto tPrev = std::chrono::steady_clock::now();
	auto tStacked = tPrev;
	std::chrono::steady_clock::time_point tPassed[2]{};

	bool shouldInterpolate = false;
	float tInterpolate = 0.0f;
	while (CheckWindowOpen()) {
		auto tCurrent = std::chrono::steady_clock::now();
		auto dt = tCurrent - tPrev;
		tPrev = tCurrent;
		tStacked = tStacked + dt;


		entity.statePrev = entity.state;

		float mouseX, mouseY;
		if (CheckMouseInput(0, InputType::pressed, &mouseX, &mouseY)) {
			if (entity.CheckPointInBody(mouseX, mouseY)) {
				entity.isSelected = true;
			}
		}
		if (CheckMouseInput(0, InputType::held, &mouseX, &mouseY)) {
			if (entity.isSelected) {
				entity.SetPosition(mouseX, mouseY);
				shouldInterpolate = false;
			}
		}
		if (CheckMouseInput(0, InputType::released, &mouseX, &mouseY)) {
			if (entity.isSelected) {
				switch (entity.place) {
					case Place::deck: {
						Place targetPlace = Place::hand;
						entity.SetTarget(400.0f, 525.0f);
					} break;
					case Place::hand: 
					case Place::field: {
						if (entity.CheckBodyOnField()) {
							float fieldX, fieldY;
							entity.PlaceOnField(&fieldX, &fieldY);
							data.x = fieldX;
							data.y = fieldY;
						}
						else {
							entity.SetTarget(400, 500);
							data.x = entity.x;
							data.y = entity.y;
						}
					} break;
				}
			}
			entity.isSelected = false;
		}

		entity.Update(dt.count() / (float) 1E9);


		if (data != dataLastFrame) {
			dataLastFrame = data;
			SendData(data);
		}
		
		if (Listen()) {
			tPassed[1] = tPassed[0];
			tPassed[0] = std::chrono::steady_clock::now();
			updateDuration = tPassed[0] - tPassed[1];
			shouldInterpolate = true;

			Receive(&packetData);
		}


		float tInterpolate = updateDuration.count() == 0 ? 0 : (float)(tStacked - (tPassed[0])).count() / updateDuration.count();
		
		BeginGraphics();
			if (shouldInterpolate) entity.Render(tInterpolate);
			else				   entity.Render(1);
		EndGraphics();
	}
	return 0;
}