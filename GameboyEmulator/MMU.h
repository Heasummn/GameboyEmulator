#pragma once
#include "common.h"
#include <string>

class MMU
{
public:
	MMU();
	~MMU();

	void load(std::string name);

	byte readByte(word address) const;

	void writeByte(word address, byte value);

private:
	// Gameboy has 2^16 bits of addressible space
	byte internalMem[0xFFFF] = { 0 };

	// A Gameboy cartridge can have up to 2MB of memory
	byte cartridgeMem[0x200000] = { 0 };

	// ROM bank currently being used, bank 0 is in internal mem so we don't worry about it
	byte currRomBank = 1;

	// 1 RAM bank = 0x2000, with a maximum of 4 ram banks
	byte ramBanks[0x8000];

	// RAM bank currently being used, not stored internally, so can be 0
	byte currRamBank = 0;

	// indicates which MBC we are using
	byte mbc;

	// Only used by MBC1
	bool ramMode;

	// Is ram enabled?
	bool ramEnabled = false;

	void controlBanking(word address, byte value);
};