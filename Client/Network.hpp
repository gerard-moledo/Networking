#ifndef NETWORK_H
#define NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>

static WSADATA wsaData;
static SOCKET Socket = INVALID_SOCKET;
static sockaddr_in serverAddr;

bool InitializeNetwork();

void SendData(void* data);

#endif