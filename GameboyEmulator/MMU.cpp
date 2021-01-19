#include "mmu.h"
#include "common.h"
#include <iostream>
#include <fstream>


MMU::MMU()
{
}


MMU::~MMU()
{
}

void MMU::load(std::string name)
{
	std::ifstream in;
	in.open(name, std::ios::binary | std::ios::in);

	if (!in)
	{
		printf("Unable to read file!");
		return;
	}

	in.read(reinterpret_cast<char*>(cartridgeMem), 0x200000);

	std::memcpy(&internalMem, &cartridgeMem, 0x8000);
	in.close();
}


byte MMU::readByte(word address) const
{
	// Reading from ROM bank
	if ((address >= 0x4000) && (address < 0x8000))
	{
		word romAddress = address - 0x4000;
		return cartridgeMem[address + currRomBank * 0x4000];
	}

	// Reading from RAM bank
	if ((address >= 0xA000) && (address < 0xC000))
	{
		word ramAddress = address - 0xA000;
		return ramBanks[ramAddress + currRamBank * 0x2000];
	}

	// Just good old normal memory
	return internalMem[address];
}

void MMU::writeByte(word address, byte value)
{
	// 0x000 -> 0x8000: ROM, unwriteable, but used for banking control
	if (address < 0x8000)
	{
		controlBanking(address, value);
	}

	// 0xE000 -> 0xFE00: ECHO RAM, echoed to 0xC000 -> 0xDE00
	else if ((address >= 0xE000) && (address < 0xFE00))
	{
		internalMem[address] = value;
		internalMem[address - 0x2000] = value;
	}

	// 0xFEA0 -> 0xFEFF: Cannot be written to
	else if (((address >= 0xFEA0) && (address < 0xFEFF))) {}

	// Everything else is fair game
	else
	{
		internalMem[address] = value;
	}
}

void MMU::controlBanking(word address, byte value)
{
	if (address < 0x2000)
	{
		if (mbc == 1 || mbc == 2)
		{
			// MBC2 requires the lower bit of the upper byte to be 0. 000
			if (mbc == 2)
			{
				if (ACCESS_BIT(address, 4) == 1)
				{
					return;
				}
			}
			byte command = value & 0xF;
			if (command == 0xA)
			{
				ramEnabled = true;
			}
			else if (command == 0x0)
			{
				ramEnabled = false;
			}
		}
	}

	// Change lower 5 bits of ROM bank
	// Note: In normal GB, banks 0x20, 0x40, and 0x60 are not available, we don't care
	else if ((address >= 0x2000) && (address < 0x4000))
	{
		if (mbc == 1 || mbc == 2)
		{
			currRomBank &= 0b11100000; // Set lower 5 bits to 0
			currRomBank |= (value & 0b00011111); // Set to lower 5 bits of value

			if (currRomBank == 0) // Rom bank 0 is already in mem, can't switch to it
			{
				currRomBank = 1;
			}
		}
	}

	// Select RAM bank or upper 2 bits of ROM bank
	if ((address >= 4000) && (address < 0x6000))
	{
		if (mbc == 1)
		{
			if (ramMode)
			{
				currRamBank = value & 0b11;
			}
			else
			{
				currRomBank &= 0b00011111; // Set high 3 bits to 0
				currRomBank |= (value & 0b11100000); // Set to high 3 bits of value

				if (currRomBank == 0)
				{
					currRomBank = 1;
				}
			}
		}
	}

	// Change between RAM or ROM modes. MBC1 ONLY!
	if ((address >= 6000) && (address < 0x8000) && (mbc == 1))
	{
		ramMode = (value & 0x1) == 0 ? false : true;
		if (!ramMode)
		{
			currRamBank = 0; // GB can only use Ram Bank 0 during ROM mode
		}
	}
}