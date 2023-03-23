#include "Network.hpp"

#include <cstdio>

namespace Network {
	WSADATA wsaData;
	SOCKET Socket = INVALID_SOCKET;

	char dataBuffer[BUFFER_SIZE];
	
	std::vector<Client> clients;
	std::vector<Game> games;

	std::queue<Packet> receiveQueue;
	std::vector<Packet> sendBacklog;
}

bool Network::Initialize() {
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0) {
		printf("WSAStartup failed: %d\n", initResult);
		return false;
	}

	Network::Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	addrinfo hints{}, *result;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int getaddrResult = getaddrinfo(NULL, "3816", &hints, &result);
	if (getaddrResult != 0) {
		printf("getaddrinfo() failed: %d\n", getaddrResult);
		return false;
	}
	
	int bindResult = bind(Network::Socket, result->ai_addr, result->ai_addrlen);
	if (bindResult == SOCKET_ERROR) {
		printf("bind() failed: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	return true;
}


bool Network::Listen(SOCKET listenSocket) {
	if (listenSocket == Network::Socket) {
		int listenResult = listen(Network::Socket, SOMAXCONN);
		if (listenResult == SOCKET_ERROR) {
			printf("listen() failed: %d\n", WSAGetLastError());
		}
	}

	fd_set socketSet{};
	FD_ZERO(&socketSet);
	FD_SET(listenSocket, &socketSet);
	timeval timeout{ 0, 0 };
	int selectResult = select(NULL, &socketSet, nullptr, nullptr, &timeout);
	if (selectResult == SOCKET_ERROR) {
		printf("select() failed: %d\n", WSAGetLastError());
	}

	return socketSet.fd_count > 0;
}

int Network::ReceivePackets(SOCKET receiveSocket) {
	int recvResult = recv(receiveSocket, Network::dataBuffer, BUFFER_SIZE, 0);
	if (recvResult == SOCKET_ERROR) {
		printf("recv() failed: %d\n", WSAGetLastError());
		return 0;
	}
	
	int numPackets = recvResult / (int)sizeof Packet;
	printf("%d\n", numPackets);
	for (int i = 0; i < numPackets; i++) {
		Network::receiveQueue.emplace(*(((Packet*) Network::dataBuffer) + i));
	}

	return recvResult;
}

void Network::SendToGame(Packet data, Game game) {
	auto itHost = std::find_if(Network::clients.begin(), Network::clients.end(),
							[&](Client& client) { return client.id == game.host.id; });
	auto itPeer = std::find_if(Network::clients.begin(), Network::clients.end(),
							[&](Client& client) { return client.id == game.peer.id; });

	if (itHost != Network::clients.end())
		Network::Send(data, *itHost);
	if (itPeer != Network::clients.end())
		Network::Send(data, *itPeer);
}

void Network::Send(Packet data, Client client) {
	int sendResult = send(client.mSocket, (char*)&data, sizeof Packet, 0);
	if (sendResult == SOCKET_ERROR) {
		printf("send() failed: %d. Client: %llu, Packet origin: %llu\n", WSAGetLastError(), client.id, data.id);
	}
}

void Network::Deinitialize() {
	closesocket(Network::Socket);
	WSACleanup();
}