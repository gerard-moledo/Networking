#ifndef LOBBY_H
#define LOBBY_H

#include "Network.hpp"
#include "Util.hpp"

namespace Lobby {
	void AddClient(uint64_t id);
	void Update(Packet* packet);
	void CreateGame(Client& host, Client& peer);
}

#endif