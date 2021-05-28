#pragma once
#include <cstdint>

// Macro definitions, mainly bit fiddling
#define ACCESS_BIT(x, bit) ((x >> bit) & 1U)
#define SET_BIT(x, bit, value) (x ^= (-(value) ^ x) & (1 << bit))
#define CLEAR_FLAG() registers.flag = 0;

// Constant definitions
static const int ZERO_FLAG = 7;
static const int SUB_FLAG = 6;
static const int HALF_CARRY_FLAG = 5;
static const int CARRY_FLAG = 4;


const unsigned int GAMEBOY_WIDTH = 160;
const unsigned int GAMEBOY_HEIGHT = 144;

// Type definitions
typedef uint8_t byte;
typedef int8_t signed_byte;
typedef uint16_t word;

inline word join_bytes(const byte high_b, const byte low_b) {
	return static_cast<word>((high_b << 8) | low_b);
}