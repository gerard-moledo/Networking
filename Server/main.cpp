#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

SOCKET Socket = INVALID_SOCKET;

int main() {
	WSADATA wsaData;
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0) {
		printf("WSAStartup failed: %d\n", initResult);
		return 1;
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
		return 1;
	}

	sockaddr_in senderAddr;
	int senderAddrSize = sizeof(senderAddr);
	char recvBuffer[512];
	int selectResult, recvResult;

	fd_set readSet;

	while (true) {
		FD_ZERO(&readSet);
		FD_SET(Socket, &readSet);
		timeval timeout{ 0, 0 };
		selectResult = select(NULL, &readSet, nullptr, nullptr, &timeout);
		if (selectResult == SOCKET_ERROR) {
			printf("select() failed: %d\n", WSAGetLastError());
			continue;
		}

		if (readSet.fd_count > 0) {
			recvResult = recvfrom(Socket, recvBuffer, 512, 0, (SOCKADDR *) & senderAddr, &senderAddrSize);
			printf("Socket read: %s\n", recvBuffer);
		}
	}

	closesocket(Socket);
	WSACleanup();

	return 0;
}