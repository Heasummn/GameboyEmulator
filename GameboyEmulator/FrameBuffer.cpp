#include "FrameBuffer.h"



FrameBuffer::FrameBuffer()
{
	for (uint32_t x = 0; x < GAMEBOY_WIDTH; x++) {
		for (uint32_t  y = 0; y < GAMEBOY_HEIGHT; y++) {
			setPixel(x, y, Color::Color0);
		}
	}
}

void FrameBuffer::setPixel(uint32_t x, uint32_t y, Color color)
{
	buffer[(y * GAMEBOY_WIDTH) + x] = color;
}

void FrameBuffer::setPixel(uint32_t x, uint32_t y, byte color)
{
	Color realColor;
	switch (color) {
	case 0: 
		realColor = Color::Color0;
		break;
	case 1:
		realColor = Color::Color1;
		break;
	case 2:
		realColor = Color::Color2;
		break;
	case 3:
		realColor = Color::Color3;
		break;
	default:
		realColor = Color::Color0;
		break;
	}
	buffer[(y * GAMEBOY_WIDTH) + x] = realColor;
}


Color FrameBuffer::getPixel(uint32_t x, uint32_t y) const
{
	return buffer[(y * GAMEBOY_WIDTH) + x];
}

Color FrameBuffer::getPixel(uint32_t pixel) const
{
	return buffer[pixel];
}


FrameBuffer::~FrameBuffer()
{
}
