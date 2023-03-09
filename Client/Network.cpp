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

	return true;
}

void SendData(void* data) {
	sendto(Socket, (char *) data, sizeof (char*), 0, (SOCKADDR*)&serverAddr, sizeof serverAddr);
}