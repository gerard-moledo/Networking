#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <iostream>
#include <bitset>
#include <chrono>

SOCKET Socket = INVALID_SOCKET;

struct Entity
{
	int x = 100;
	int y = 100;
	int vx = 0;
	int vy = 0;
};

struct Tick
{
	float acc;
	float step;
};

struct Packet
{
	int x, y;
};

WSADATA wsaData;

bool InitializeNetwork() {
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0) {
		printf("WSAStartup failed: %d\n", initResult);
		return false;
	}

	sockaddr_in RecvAddr;

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(8080);
	RecvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int bindResult = bind(Socket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (bindResult == SOCKET_ERROR) {
		printf("bind() failed: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	return true;
}

int main() {
	int networkInitialized = InitializeNetwork();
	if (!networkInitialized) {
		printf("Network initialization failed.\n");
		return 1;
	}

	// Network vars
	sockaddr_in senderAddr;
	int senderAddrSize = sizeof senderAddr;
	char dataBuffer[8];
	int selectResult, recvResult, sendResult;
	fd_set readSet;

	// Gameplay vars
	Entity entity;

	// Game Loop vars
	auto tCurrent = std::chrono::high_resolution_clock::now();
	auto tPrev = tCurrent;
	Tick physicsTick = { 0.0f, 0.02f };
	Tick networkTick = { 0.0f, 0.04f };

	// GAME LOOP
	while (true) {
		tCurrent = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration_cast<std::chrono::microseconds>(tCurrent - tPrev).count() / 1000000.0f;
		tPrev = tCurrent;
		physicsTick.acc += dt;
		networkTick.acc += dt;

		FD_ZERO(&readSet);
		FD_SET(Socket, &readSet);
		timeval timeout{ 0, 0 };
		selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
		if (selectResult == SOCKET_ERROR) {
			printf("select() failed: %d\n", WSAGetLastError());
			continue;
		}
		if (readSet.fd_count > 0) {
			recvResult = recvfrom(Socket, dataBuffer, 8, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			if (recvResult == SOCKET_ERROR) {
				printf("recvfrom() failed: %d\n", WSAGetLastError());
				return 1;
			}

			char inputBuffer = dataBuffer[0];
			entity.vx = 0;
			entity.vy = 0;
			if (inputBuffer & (1 << 0)) entity.vx += 10;
			if (inputBuffer & (1 << 1)) entity.vy += 10;
			if (inputBuffer & (1 << 2)) entity.vx += -10;
			if (inputBuffer & (1 << 3)) entity.vy += -10;

			std::cout << "Input read: " << std::bitset<8>(inputBuffer) << std::endl;
			printf("%d\n", entity.x);
		}


		while (physicsTick.acc > physicsTick.step) {
			physicsTick.acc -= physicsTick.step;
			
			entity.x += entity.vx;
			entity.y += entity.vy;
		}
		if (networkTick.acc > networkTick.step) {
			networkTick.acc = 0;

			Packet packet{ entity.x, entity.y };
			sendResult = sendto(Socket, (char *) &packet, sizeof packet, 0, (SOCKADDR *) &senderAddr, senderAddrSize);
		}
	}

	closesocket(Socket);
	WSACleanup();

	return 0;
}