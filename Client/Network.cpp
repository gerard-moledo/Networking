#include "Network.hpp"

#include <stdio.h>
#include <iostream>

bool InitializeNetwork() {

	int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0) {
		printf("WSAStartup() failed: %d", startupResult);
		return false;
	}

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET) {
		printf("socket() failed: %d", WSAGetLastError());
		WSACleanup();
		return false;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.S_un.S_addr);

	for (int i = 0; i < 64; i++) {
		sessionId += ((unsigned long long) (rand() % 2)) << i;
		printf("Session: %llx\n", sessionId);
	}
	
	return true;
}

uint64_t GetSessionId() {
	return sessionId;
}

void SendData(Packet data) {
	sendto(Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&serverAddr, sizeof serverAddr);
}

bool Listen() {
	FD_ZERO(&readSet);
	FD_SET(Socket, &readSet);
	timeval timeout{ 0, 0 };
	int selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
		return false;
	}

	return readSet.fd_count > 0;
}

void Receive(Packet* packet) {
	int recvResult = recvfrom(Socket, data, 64, 0, (SOCKADDR*)&serverAddr, &serverAddrSize);
	if (recvResult == SOCKET_ERROR) {
		printf("recvfrom() failed: %d\n", WSAGetLastError());
		return;
	}

	*packet = *(Packet*)data;
}