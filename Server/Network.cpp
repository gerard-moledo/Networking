#include "Network.hpp"

#include <cstdio>

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	sockaddr_in senderAddress;
	int senderAddressSize = sizeof sockaddr_in;
	char dataBuffer[BUFFER_SIZE];
	
	std::vector<Client> clients;
	std::vector<Game> games;

	std::vector<Packet> sendQueue;
}

bool Network::Initialize() {
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0) {
		printf("WSAStartup failed: %d\n", initResult);
		return false;
	}

	Network::Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(3816);
	RecvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int bindResult = bind(Network::Socket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (bindResult == SOCKET_ERROR) {
		printf("bind() failed: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	return true;
}


bool Network::Listen() {
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(Network::Socket, &socketSet);
	timeval timeout{ 0, 0 };
	int selectResult = select(NULL, &socketSet, nullptr, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
	}

	return socketSet.fd_count > 0;
}

Packet* Network::ReceivePacket() {
	int recvResult = recvfrom(Network::Socket, Network::dataBuffer, BUFFER_SIZE, 0, (SOCKADDR*)&Network::senderAddress, &Network::senderAddressSize);
	if (recvResult == SOCKET_ERROR) {
		printf("recvfrom() failed: %d\n", WSAGetLastError());
		return nullptr;
	}
	
	 return (Packet*)Network::dataBuffer;
}

void Network::SendToGame(Packet data, Game currentGame) {
	for (Game& game : Network::games) {
		if (game.id == currentGame.id) {
			auto itHost = std::find_if(Network::clients.begin(), Network::clients.end(),
									[&](Client& client) { return client.id == game.host.id; });
			auto itPeer = std::find_if(Network::clients.begin(), Network::clients.end(),
									[&](Client& client) { return client.id == game.peer.id; });
			if (itHost != Network::clients.end())
				Network::Send(data, *itHost);
			if (itPeer != Network::clients.end())
				Network::Send(data, *itPeer);
		}
	}
}

void Network::Send(Packet data, Client client) {
	int sendResult = sendto(Network::Socket, (char*)&data, sizeof Packet, 0, (SOCKADDR*)&client.address, sizeof client.address);
	if (sendResult == SOCKET_ERROR) {
		printf("sendto() failed: %d. Client: %llu, Packet origin: %llu\n", WSAGetLastError(), client.id, data.id);
	}
}

void Network::Deinitialize() {
	closesocket(Network::Socket);
	WSACleanup();
}