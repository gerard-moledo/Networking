#include "Network.hpp"

#include <stdio.h>

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	std::vector<Client> clients;
	sockaddr_in senderAddr;
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
	RecvAddr.sin_port = htons(3816);
	printf("port: %d\n", RecvAddr.sin_port);
	RecvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int bindResult = bind(Socket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (bindResult == SOCKET_ERROR) {
		printf("bind() failed: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	return true;
}

Client Network::CheckForClient() {
	if (Network::Listen()) {
		printf("Received!\n");
		Packet dataReceived;
		Receive(&dataReceived);
		
		uint64_t id = dataReceived.status == ConnectionStatus::none ? dataReceived.id : 0;
		return Client{ dataReceived.id, ConnectionStatus::toServer, senderAddr };
	}

	return Client{};
}

bool Network::AddOrRemoveClient(Client& client) {
	if (!CheckClientExists(client.id)) {
		AddClient(client);
		Send(Packet{ client.id, ConnectionStatus::none });
		return true;
	}

	Send(Packet{ client.id, ConnectionStatus::none });

	return false;
}

bool Network::CheckClientExists(uint64_t id) {
	return std::find_if(clients.begin(), clients.end(), [&](Client client) { return id == client.id; }) != clients.end();
}

void Network::AddClient(Client clientToAdd) {
	clients.emplace_back(clientToAdd);
}

void Network::RemoveClient(Client clientToRemove) {
	auto itRemove = std::remove_if(clients.begin(), clients.end(), [&](Client client) { return clientToRemove.id == client.id; });
	
	if (itRemove != clients.end())  
		clients.erase(itRemove);
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

void Network::Receive(Packet* packetData) {
	int recvResult = recvfrom(Socket, dataBuffer, sizeof dataBuffer, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);
	if (recvResult == SOCKET_ERROR) {
		printf("recvfrom() failed: %d\n", WSAGetLastError());
		*packetData = Packet{};
		return;
	}
	*packetData = *(Packet*)dataBuffer;
}

void Network::Send(Packet data) {
	for (Client& client : clients) {
		int sendResult = sendto(Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&client.addr, sizeof client.addr);
		if (sendResult == SOCKET_ERROR) {
			printf("sendto() failed: %d\n", sendResult);
			return;
		}
	}
}

void Network::Deinitialize() {
	closesocket(Socket);
	WSACleanup();
}