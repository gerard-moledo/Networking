#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

enum struct Place : unsigned char { deck, hand, field };
enum struct Type : unsigned char { white, black, green, blue };
enum struct Phase : unsigned char { debug1, start, draw, play, end, wait, debug2 };
struct CardState
{
	uint16_t id;

	Place place;
	uint8_t index;

	Type type;
};

#endif