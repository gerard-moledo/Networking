#ifndef NETWORK_H
#define NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>

static WSADATA wsaData;
static SOCKET Socket = INVALID_SOCKET;
static sockaddr_in serverAddr;
static int serverAddrSize = sizeof serverAddr;
static fd_set readSet;
static char data[8];

struct Packet
{
	int x, y;
};

bool InitializeNetwork();

void SendData(char data);
bool Listen();
void Receive(Packet* packet);

#endif