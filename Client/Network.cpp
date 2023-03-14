#include "Network.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	sockaddr_in serverAddr;
	int serverAddrSize = sizeof serverAddr;
	fd_set readSet;

	char data[256];

	uint64_t sessionId;
}

bool Network::Initialize() {

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
	inet_pton(AF_INET, "173.2.44.88", &serverAddr.sin_addr.S_un.S_addr);

	for (int i = 0; i < 64; i++) {
		sessionId += ((unsigned long long) (rand() % 2)) << i;
		printf("Session: %llx\n", sessionId);
	}
	
	return true;
}

int Network::WaitForClients() {
	if (Network::Listen()) {
		int recvResult = recvfrom(Socket, data, sizeof data, 0, (SOCKADDR*)&serverAddr, &serverAddrSize);
		if (recvResult == SOCKET_ERROR) {
			printf("recvfrom() failed: %d\n", WSAGetLastError());
			return 0;
		}
		uint64_t idHost = *(uint64_t*)data;

		return idHost == sessionId ? 1 : 2;
	}

	Network::SendData(Packet{ Network::sessionId });
	return -1;
}

void Network::SendData(Packet data) {
	sendto(Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&serverAddr, sizeof serverAddr);
}

bool Network::Listen() {
	FD_ZERO(&readSet);
	FD_SET(Socket, &readSet);
	timeval timeout{ 0, 0 };
	int selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
		return false;
	}

	sizeof Packet;
	return readSet.fd_count > 0;
}

void Network::Receive(Packet* packet) {
	int recvResult = recvfrom(Socket, data, sizeof data, 0, (SOCKADDR*)&serverAddr, &serverAddrSize);
	if (recvResult == SOCKET_ERROR) {
		printf("recvfrom() failed: %d\n", WSAGetLastError());
		return;
	}

	*packet = *(Packet*)data;
}