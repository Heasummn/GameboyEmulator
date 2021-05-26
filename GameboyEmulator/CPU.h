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

	typedef struct {
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
	} registers_t;

	inline registers_t getRegisters() { return registers; };
private:

	
	registers_t registers;
	std::unique_ptr<MMU> mmu;
	byte* byteRegisters[8];

	void setByteRegisterVal(byte dst, byte val);
	void setByteRegisters(byte src, byte dst);
	byte getByteRegister(byte reg_num);
	word getWordAtPC();

	void stackPush(word reg);
	void stackPop(word& reg);

	// opcodes
	void INC_DEC_reg8(byte opcode);
	void INC_DEC_reg16(byte opcode);
	void LD_r8(byte opcode);
	void LD_r16(byte opcode);
	void LD_address_acc(byte opcode);
	void LD_acc_data();
	void LD_acc_address(byte opcode);
	void LD_r_r(byte opcode);
	void LD_address_r(byte opcode);
	void LD_r_address(byte& opcode);
	void ALU_r(byte opcode);
	void JR();
	void JR(bool flag);
	void JP_16();
	void RET();
	void CALL_16();
	void RST(word address);
};

