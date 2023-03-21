#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

enum struct Place : unsigned char { deck, hand, field };
enum struct Type : unsigned char { white, black, green, blue };
enum struct Phase : unsigned char { start, draw, play, end, wait };

struct CardState
{
	uint16_t id;

	bool isSelected;
	Place place;
	uint8_t index;

	Type type;
};

enum struct ConnectionState : uint8_t { none, lobby, game, disconnected };

struct Packet
{
	uint64_t id;
	ConnectionState state;
	bool isTurn;
	CardState cards[10];
};

struct Tick
{
	float acc;
	float step;
};

#endif