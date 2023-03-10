#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <iostream>
#include <bitset>
#include <chrono>

SOCKET Socket = INVALID_SOCKET;

struct Tick
{
	float acc;
	float step;
};

struct Packet
{
	float x, y;
};

struct EntityState
{
	float x;
	float y;
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
	char dataBuffer[64];
	fd_set readSet;

	// Gameplay vars
	EntityState entityState{400, 500};

	// Game Loop vars
	auto tCurrent = std::chrono::high_resolution_clock::now();
	auto tPrev = tCurrent;
	Tick physicsTick = { 0.0f, 0.02f };
	Tick networkTick = { 0.0f, 0.04f };

	// GAME LOOP
	while (true) {
		tCurrent = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration_cast<std::chrono::microseconds>(tCurrent - tPrev).count() / (float) 1E6;
		tPrev = tCurrent;
		physicsTick.acc += dt;
		networkTick.acc += dt;

		FD_ZERO(&readSet);
		FD_SET(Socket, &readSet);
		timeval timeout{ 0, 0 };
		int selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
		if (selectResult == SOCKET_ERROR) {
			printf("select() failed: %d\n", WSAGetLastError());
			continue;
		}
		if (readSet.fd_count > 0) {
			int recvResult = recvfrom(Socket, dataBuffer, 64, 0, (SOCKADDR *) &senderAddr, &senderAddrSize);
			if (recvResult == SOCKET_ERROR) {
				printf("recvfrom() failed: %d\n", WSAGetLastError());
				return 1;
			}

			Packet data = *(Packet*) dataBuffer;
			entityState.x = data.x;
			entityState.y = data.y;
		}


		while (physicsTick.acc > physicsTick.step) {
			physicsTick.acc -= physicsTick.step;
			
		}
		if (networkTick.acc > networkTick.step) {
			networkTick.acc = 0;

			Packet data;
			data.x = entityState.x;
			data.y = entityState.y;
			int sendResult = sendto(Socket, (char*) &data, NULL, 0, (SOCKADDR *) &senderAddr, senderAddrSize);
		}
	}

	closesocket(Socket);
	WSACleanup();

	return 0;
}