#pragma once
#include <vector>
#include "common.h"

class Tile
{
public:
	Tile();
	Tile(byte line, std::vector<byte> pixels);
	~Tile();

	byte line;
	std::vector<byte> pixels;
};

