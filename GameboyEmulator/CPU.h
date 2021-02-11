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

	void setByteRegisters(byte src, byte dst);
	byte getByteRegister(byte reg_num);
	word getWordAtPC();

	void stackPush(word reg);
	void stackPop(word& reg);

	// opcodes
	void LD_r16(byte opcode);
	void LD_r_r(byte opcode);
	void ALU_r(byte opcode);
	void JP_16();
	void RET();
	void CALL_16();
	void RST(word address);
};

