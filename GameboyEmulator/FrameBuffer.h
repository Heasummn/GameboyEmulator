#pragma once
#include "common.h"

enum class Color {
	Color0, 
	Color1,
	Color2,
	Color3
};

class FrameBuffer
{
public:
	FrameBuffer();
	void setPixel(uint32_t x, uint32_t y, Color color);
	void setPixel(uint32_t x, uint32_t y, byte color);
	Color getPixel(uint32_t  x, uint32_t y) const;
	Color getPixel(uint32_t  pixel) const;

	~FrameBuffer();
private: 
	Color buffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH];
};

