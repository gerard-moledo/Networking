#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

SOCKET RecvSocket = INVALID_SOCKET;

bool InitServer() {
	WSADATA wsaData;
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0) {
		printf("WSAStartup failed: %d\n", initResult);
		return false;
	}

	sockaddr_in RecvAddr;

	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(8080);
	RecvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int bindResult = bind(RecvSocket, (SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
	if (bindResult == SOCKET_ERROR) {
		printf("bind() failed: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	return true;
}

int main() {
	int initialized = InitServer();
	if (!initialized) {
		printf("Server initialization failed.\n");
		return 1;
	}

	sockaddr_in senderAddr;
	int senderAddrSize = sizeof(senderAddr);
	char recvBuffer[512];
	int recvResult;
	do {
		recvResult = recvfrom(RecvSocket, recvBuffer, 512, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);
		if (recvResult > 0) {
			printf("Bytes received: %d\n", recvResult);
			printf("Message: %s\n", recvBuffer);
		}
		else if (recvResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("recv failed: %ld\n", WSAGetLastError());
			closesocket(RecvSocket);
			WSACleanup();
			return 1;
		}
	} while (recvResult > 0);

	closesocket(RecvSocket);
	WSACleanup();

	return 0;
}