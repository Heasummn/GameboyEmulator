#pragma once

#include <memory>
#include "common.h"
#include "MMU.h"

class CPU
{
public:
	CPU();
	~CPU();
	void loadRom(std::string name);
	void step();
private:

	struct {
		// BC, DE, and HL registers are paired together as 16 bit
		union
		{
			word bc;
			struct {
				byte c, b;
			};
		};

		union
		{
			word de;
			struct {
				byte e, d;
			};
		};

		union
		{
			word hl;
			struct {
				byte l, h;
			};
		};
		byte acc;

		// Flag register format: Z N H C XXX
		byte flag;

		word sp;
		word pc;
	} registers;
	std::unique_ptr<MMU> mmu;
	byte* byteRegisters[8];

	byte* getByteRegister(byte num);

	// opcodes
	void LD_r16(byte opcode);
	void LD_r_r(byte opcode);
	void ALU_r(byte opcode);
};

