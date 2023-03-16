#include "Network.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ws2def.h>

//#define USE_DOMAIN

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	sockaddr_in serverAddr;
	int serverAddrSize = sizeof serverAddr;
	fd_set readSet;

	char data[256];

	uint64_t sessionId;

	float tLastReceived;
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


	ADDRINFOA hints{ };
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

#ifdef USE_DOMAIN
	ADDRINFOA* result;
	int addrResult = getaddrinfo("cataclysmwar.com", "3816", &hints, &result);
	if (addrResult != 0) {
		printf("getaddrinfo() failed: %d", WSAGetLastError());
		return false;
	}
	serverAddr = *(sockaddr_in*)result->ai_addr;
#else
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(3816);
	int ptonResult = inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.S_un.S_addr);
	if (ptonResult != 1) {
		printf("inet_pton() failed: %d\n", ptonResult);
		if (ptonResult == 1) {
			printf("Additional error: %d\n", WSAGetLastError());
		}
	}
#endif
	for (int i = 0; i < 64; i++) {
		sessionId += ((unsigned long long) (rand() % 2)) << i;
	}
	
	return true;
}

bool Network::CheckServerConnected() {
	if (Network::Listen(1000)) {
		int recvResult = recvfrom(Socket, data, sizeof data, 0, (SOCKADDR*)&serverAddr, &serverAddrSize);
		if (recvResult == SOCKET_ERROR) {
			printf("recvfrom() failed: %d\n", WSAGetLastError());
			return true;
		}
		Packet packet = *(Packet*)data;
		return packet.status == ConnectionStatus::none;
	}

	printf("Server Send\n");
	Packet data;
	data.id = Network::sessionId;
	data.status = ConnectionStatus::none;
	Network::Send(data);
	return false;
}

bool Network::CheckClientConnected() {
	if (Network::Listen(1000)) {
		int recvResult = recvfrom(Socket, data, sizeof data, 0, (SOCKADDR*)&serverAddr, &serverAddrSize);
		if (recvResult == SOCKET_ERROR) {
			printf("recvfrom() failed: %d\n", WSAGetLastError());
			return 0;
		}
		Packet packet = *(Packet*)data;
		return packet.id != Network::sessionId && packet.status == ConnectionStatus::toServer;
	}
	
	printf("Client Send\n");
	Packet data;
	data.id = Network::sessionId;
	data.status = ConnectionStatus::toServer;
	Network::Send(data);
	return false;
}

void Network::Send(Packet data) {
	int sendResult = sendto(Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&serverAddr, sizeof serverAddr);
	if (sendResult == SOCKET_ERROR) {
		printf("sendto() failed %d\n", WSAGetLastError());
		return;
	}
}

bool Network::Listen(int waitTime) {
	FD_ZERO(&readSet);
	FD_SET(Socket, &readSet);
	timeval timeout{ 0, waitTime };
	int selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
		return false;
	}

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

void Network::Deinitialize() {
	Packet data;
	data.id = sessionId;
	data.status = ConnectionStatus::none;
	Send(data);

	closesocket(Socket);
	WSACleanup();
}