#pragma once

#include <memory>
#include "common.h"
#include "opcode_timings.h"
#include "GPU.h"
#include "MMU.h"

class CPU
{
public:
	CPU(draw_callback_t draw_func);
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

	int instTime; // TODO: not sure what the best type for this is
	MMU mmu;
	GPU gpu;

private:

	
	registers_t registers;

	byte* byteRegisters[8];


	int clockTime; // TODO: not sure what the best type for this is
	bool branchTaken;

	int stepNormalOpcodes(byte opcode);
	int stepExtendedOpcodes(byte opcode);

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
	void LD_data_acc();
	void LD_acc_address(byte opcode);
	void LD_Cadd_acc();
	void LD_r_r(byte opcode);
	void LD_address_r(byte opcode);
	void LD_r_address(byte& opcode);
	void ALU_r(byte opcode);
	void JR();
	void JR(bool flag);
	void JP_16();
	void RET();
	void CALL_16();
	void CP_A8();
	void RST(word address);

	void SR(byte opcode);
	void BIT(byte opcode);
};

