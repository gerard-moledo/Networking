#include "Network.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ws2def.h>

#define SERVER_DOMAIN "cataclysmwar.com"

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;
	sockaddr_in serverAddr;
	int serverAddrSize = sizeof serverAddr;

	std::queue<Packet> packetQueue;
	fd_set readSet;
	fd_set writeSet;

	char dataBuffer[BUFFER_SIZE];

	uint64_t sessionId;
	ConnectionState state = ConnectionState::none;
}

bool Network::Initialize() {

	int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0) {
		printf("WSAStartup() failed: %d", startupResult);
		return false;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET) {
		printf("socket() failed: %d", WSAGetLastError());
		WSACleanup();
		return false;
	}


	ADDRINFOA hints{ };
	//hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFOA* result;
	int addrResult = getaddrinfo(SERVER_DOMAIN, "3816", &hints, &result);
	if (addrResult != 0) {
		printf("getaddrinfo() failed: %d", WSAGetLastError());
		return false;
	}
	serverAddr = *(sockaddr_in*)result->ai_addr;

	freeaddrinfo(result);

	u_long unblock = 1;
	ioctlsocket(Socket, FIONBIO, &unblock);

	for (int i = 0; i < 64; i++) {
		sessionId += ((uint64_t) (rand() % 2)) << i;
	}
	
	return true;
}

void Network::SendState() {
	Packet data{};
	data.id = Network::sessionId;
	data.state = ConnectionState::game;
	data.phase = Game::phase;
	for (size_t i = 0; i < Game::host.cards.size(); i++) {
		data.cards[i] = Game::host.cards[i].state;
	}
	Network::Send(data);
}

void Network::Send(Packet data) {
	printf("SENT\n");
	int sendResult = send(Socket, (char*)&data, sizeof Packet, 0);
	if (sendResult == SOCKET_ERROR) {
		printf("send() failed %d\n", WSAGetLastError());
		return;
	}
}

bool Network::Listen(bool checkRead, bool checkWrite) {
	FD_ZERO(&readSet);
	if (checkRead) FD_SET(Socket, &readSet);

	FD_ZERO(&writeSet);
	if (checkWrite) FD_SET(Socket, &writeSet);
	timeval timeout{ 0, 0 };
	int selectResult = select(NULL, &readSet, &writeSet, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
		return false;
	}

	return checkRead && readSet.fd_count > 0 || checkWrite && writeSet.fd_count > 0;
}

void Network::ReceivePackets() {
	int recvResult = recv(Socket, dataBuffer, sizeof dataBuffer, 0);
	if (recvResult == SOCKET_ERROR) {
		printf("recv() failed: %d\n", WSAGetLastError());
		return;
	}

	int numPackets = recvResult / (int) sizeof Packet;

	//printf("Received: %d\n", recvResult);
	for (int i = 0; i < numPackets; i++) {
		Packet data = *(((Packet*)dataBuffer) + i);
		Network::packetQueue.emplace(data);
	}
}

void Network::Deinitialize() {
	Packet data{};
	data.id = sessionId;
	data.state = ConnectionState::disconnected;
	Send(data);

	closesocket(Socket);
	WSACleanup();
}