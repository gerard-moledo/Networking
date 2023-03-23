#ifndef LOBBY_H
#define LOBBY_H

#include "Network.hpp"
#include "Util.hpp"

namespace Lobby {
	void AddClient(SOCKET clientSocket);
	void HandleDisconnections(Packet packet);
	void Update(Packet packet);
	void CreateGame(Client& host, Client& peer);
}

#endif