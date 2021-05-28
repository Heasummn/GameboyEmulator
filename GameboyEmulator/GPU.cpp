#include "GPU.h"

GPU::GPU(MMU& mmu, draw_callback_t draw_func)
{
	this->mmu = mmu;
	draw = draw_func;
	mode = VideoMode::HBLANK;
	modeClock = 0;
	line = 0;
	scrollX = 0;
	scrollY = 0;
}


GPU::~GPU()
{
}

void GPU::step(int time) {
	modeClock += time;

	switch (mode) {
	// Hblank
	case VideoMode::HBLANK:
		if (modeClock >= 204) {
			modeClock = 0;
			line++;
			if (line == 143) {
				mode = VideoMode::VBLANK;
				draw(frame);
			}
			else {
				mode = VideoMode::OAM_READ;
			}
		}
		break;

	// Vblank
	case VideoMode::VBLANK:
		if (modeClock >= 456) {
			modeClock = 0;
			line++;
			if (line > 153) {
				mode = VideoMode::OAM_READ;
				line = 0;
			}
		}
		break;

	// OAM reading
	case VideoMode::OAM_READ:
		if (modeClock >= 80) {
			modeClock = 0;
			mode = VideoMode::VRAM_READ;
		}
		break;

	// VRAM reading
	case VideoMode::VRAM_READ:
		if (modeClock >= 172) {
			modeClock = 0;
			mode = VideoMode::HBLANK;

			// write a line to the buffer
			writeLine();
		}
		break;
	}
}

void GPU::writeLine() {
	uint32_t mapOffset = 0x1C00; // this is toggled

	uint32_t yStart = line + scrollY;
	uint32_t xStart = scrollX;

	uint32_t tileY = yStart / TILE_HEIGHT;
	uint32_t tileX = xStart / TILE_WIDTH;

	uint32_t tileIndex = tileY * TILES_PER_LINE + tileX;

	uint32_t y = yStart & 0b111;
	uint32_t x = xStart & 0b111;

	uint32_t tileAddress = TILE_MAP_ZERO + tileIndex; // TODO: there are two possible maps and we have to swap
	byte tileId = mmu.readByte(tileAddress);

	Tile currentTile = getTile(TILE_SET_ZERO, tileId, y);


	for (uint32_t screenX = 0; screenX < GAMEBOY_WIDTH; screenX++) {
		byte pixel_value = currentTile.pixels[x];
		frame.setPixel(screenX, line, pixel_value);
		
		x++;

		// new tile
		if (x == 8) {
			x = 0;

			tileIndex = tileIndex + 1;
			tileAddress = TILE_MAP_ZERO + tileIndex;

			tileId = mmu.readByte(tileAddress);

			currentTile = getTile(TILE_SET_ZERO, tileId, y);
		}
	}
}

Tile GPU::getTile(uint32_t tileLocation, byte tileId, byte tileLine) {
	uint32_t offset = tileId * TILE_BYTES;
	uint32_t tileAddress = tileLocation + offset;

	uint32_t index = 2 * tileLine;
	uint32_t lineAddress = tileAddress + index;

	byte pixel1 = mmu.readByte(lineAddress);
	byte pixel2 = mmu.readByte(lineAddress + 1);

	std::vector<byte> pixels;
	for (byte i = 0; i < 8; i++) {
		byte color_value = static_cast<byte>((ACCESS_BIT(pixel2, 7 - i) << 1) | ACCESS_BIT(pixel1, 7 - i));
		pixels.push_back(color_value);
	}

	return Tile(tileLine, pixels);

}