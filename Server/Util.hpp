#ifndef UTIL_H
#define UTIL_H

#include <cstdint>

enum struct Place : uint8_t { deck, hand, field };
enum struct Type : uint8_t { white, black, green, blue };

struct CardState
{
	uint16_t id;

	bool isSelected;
	Place place;
	uint8_t index;

	Type type;
};

enum struct ConnectionState : uint8_t { none, lobby, game, disconnected };
enum struct Phase : uint8_t { start, draw, play, end, wait };

struct Packet {
	uint64_t id;
	ConnectionState state;
	Phase phase;
	CardState cards[10];
};

#endif