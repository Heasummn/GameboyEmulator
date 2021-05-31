#pragma once

#include <cstdint>
#include <memory>
#include <functional>

#include "common.h"
#include "MMU.h"
#include "Tile.h"
#include "FrameBuffer.h"

enum class VideoMode {
	OAM_READ,
	VRAM_READ,
	HBLANK,
	VBLANK,
};

typedef std::function<void (FrameBuffer&)> draw_callback_t;


// TODO: figure something out with these
const uint32_t TILE_SET_ZERO = 0x8000;
const uint32_t TILE_MAP_ZERO = 0x9800;
const uint32_t TILES_PER_LINE = 32;
const uint32_t TILE_HEIGHT = 8;
const uint32_t TILE_WIDTH = 8;
const uint32_t TILE_BYTES = 2 * 8;


class GPU
{
public:
	GPU(MMU& mmu, draw_callback_t draw_func);
	~GPU();

	void step(int time);
	byte line; // register LY
	int scrollY;
	int scrollX;

private: 
	MMU& mmu;
	FrameBuffer frame; 

	VideoMode mode;

	// TODO: Compress all of these to the right type eventually
	int modeClock;

	

	void writeLine();
	Tile getTile(uint32_t tileLocation, byte tileId, byte tileLine);

	draw_callback_t draw;
};

