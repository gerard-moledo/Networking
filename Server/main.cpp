#include "Network.hpp"
#include "Lobby.hpp"
#include "Util.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <algorithm>

void Exit();

int main() {
	srand(time(NULL));
	atexit(Exit);

	bool isNetworkInitialized = Network::Initialize();
	if (!isNetworkInitialized) {
		printf("Network initialization failed.");
		exit(1);
	}

	while (true) {
		if (Network::Listen(Network::Socket)) {
			SOCKET newSocket = accept(Network::Socket, nullptr, nullptr);
			if (newSocket == INVALID_SOCKET) {
				printf("accept() failed: %d\n", WSAGetLastError());
			}
			else {
				Lobby::AddClient(newSocket);
			}
		}
		
		for (Client& client : Network::clients) {
			if (Network::Listen(client.mSocket)) {
				int bytesReceived = Network::ReceivePackets(client.mSocket);
				if (bytesReceived > 0) 
					client.id = Network::receiveQueue.back().id;
			}
		}

		while (!Network::receiveQueue.empty()) {
			Packet packet = Network::receiveQueue.front();
			Network::receiveQueue.pop();

			Lobby::HandleDisconnections(packet);

			Lobby::Update(packet);

			for (Game& game : Network::games) {
				if (packet.state == ConnectionState::game && game.IsAPlayer(packet.id)) {
					game.Update(packet);
				} 
			}
			for (Game& game : Network::games) {
				for (auto it = Network::sendBacklog.begin(); it != Network::sendBacklog.end();) {
					if (game.IsAPlayer(it->id)) {
						Network::SendToGame(*it, game);
						it = Network::sendBacklog.erase(it);
						continue;
					}
					it++;
				}
			}
		}
	}

	exit(0);
}

void Exit() {
	Network::Deinitialize();
}