#include "Network.hpp"

#include <stdio.h>

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	std::vector<Client> clients;
	int senderAddrSize = sizeof sockaddr_in;
	char dataBuffer[256];
	fd_set readSet;
}

bool Network::Initialize() {
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

void Network::Deinitialize() {
	closesocket(Socket);
	WSACleanup();
}

bool Network::CheckClientExists(uint64_t id) {
	return std::find_if(clients.begin(), clients.end(), [&](Client client) { return id == client.id; }) != clients.end();
}

void Network::AddClient(Client client) {
	clients.emplace_back(client);
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
	
	return readSet.fd_count > 0;
}

Client Network::Receive(Packet* packetData) {
	sockaddr_in senderAddr;
	int recvResult = recvfrom(Socket, dataBuffer, sizeof dataBuffer, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);
	if (recvResult == SOCKET_ERROR) {
		printf("recvfrom() failed: %d\n", WSAGetLastError());
		return Client{};
	}

	*packetData= *(Packet*)dataBuffer;
	return Client{ packetData->id, senderAddr };
}

void Network::Send(Packet data) {
	for (Client& client : clients) {
		sendto(Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&client.addr, sizeof client.addr);
	}
}