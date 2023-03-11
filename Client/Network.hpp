#ifndef NETWORK_H
#define NETWORK_H

#include "Entity.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

static WSADATA wsaData;
static SOCKET Socket = INVALID_SOCKET;

static sockaddr_in serverAddr;
static int serverAddrSize = sizeof serverAddr;
static fd_set readSet;

static char data[64];

static uint64_t sessionId;

struct Packet
{
	uint64_t id;

	int state;
	std::vector<CardState> cards;
};

bool InitializeNetwork();

uint64_t GetSessionId();

void SendData(Packet data);
bool Listen();
void Receive(Packet* packet);

#endif