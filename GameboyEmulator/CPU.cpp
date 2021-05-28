#include "CPU.h"
#include <iostream>

CPU::CPU(draw_callback_t draw_func) : mmu(), gpu(mmu, draw_func)
{
	branchTaken = false;
	clockTime = 0;
	instTime = 0;
	registers = { 0 };
	registers.pc = 0x0100;
	registers.sp = 0xFFFE;
	
	byteRegisters[0] = &registers.b;
	byteRegisters[1] = &registers.c;
	byteRegisters[2] = &registers.d;
	byteRegisters[3] = &registers.e;
	byteRegisters[4] = &registers.h;
	byteRegisters[5] = &registers.l;
	byteRegisters[6] = &registers.acc; // this should never be used
	byteRegisters[7] = &registers.acc;
};

CPU::~CPU()
{
}

// TODO: Parsing the gameboy opcodes is fine for now, I will probably want to do a major rewrite of this to use a map or something else later though and it might be easier to just hand-write the opcodes
void CPU::step()
{
	branchTaken = false;
	byte opcode = mmu.readByte(registers.pc++);

	if (registers.pc > 0x1000) {
		std::cout << "PC: " << registers.pc << ", " << std::hex << static_cast<int>(opcode) << std::endl;
	}

	if (registers.pc == 0x2817) {
		std::cout << "starting tileset load" << std::endl; 
		return;
	}

	switch (opcode) {
	case 0x00:
		// nop
		break;

	// LD 16 bit
	case 0x01: case 0x11: case 0x21: case 0x31:
		LD_r16(opcode);
		break;

	// LD acc into address at register
	case 0x02: case 0x12: case 0x22: case 0x32:
		LD_address_acc(opcode);
		break;

	case 0x03: case 0x0B: case 0x13: case 0x1B: case 0x23: case 0x2B: case 0x33: case 0x3B:
		INC_DEC_reg16(opcode);
		break;

	// INC/DEC register
	case 0x04: case 0x05: case 0x0C: case 0x0D: case 0x14: case 0x15: case 0x1C: case 0x1D: case 0x24: case 0x25: case 0x2C: case 0x2D: case 0x34: case 0x35: case 0x3C: case 0x3D:
		INC_DEC_reg8(opcode);
		break;
	
	// LD address from PC into r8
	case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: case 0x36: case 0x3E:
		LD_r8(opcode);
		break;

	// LD address in register into acc 
	case 0x0A: case 0x1A: case 0x2A: case 0x3A:
		LD_acc_address(opcode);
		break;
	
	case 0x20:
		JR(ACCESS_BIT(registers.flag, ZERO_FLAG) == 0);
		break;
	case 0x30: 
		JR(ACCESS_BIT(registers.flag, CARRY_FLAG) == 0);
		break;

	// 0b01rrrRRR, ignoring HALT
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47: case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f: case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57: case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f: case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f: case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		LD_r_r(opcode);
		break;
	case 0x76:
		break; // HALT
	
	// ALU operations 0b10___RRR
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87: case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97: case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f: case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7: case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf: case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7: case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		ALU_r(opcode);
		break;
	
	// JP 16 bit
	case 0xC3:
		JP_16();
		break;
	
	// RETURN
	case 0xC9:
		RET();
		break;

	// CALL 16 bit
	case 0xCD:
		CALL_16();
		break;

	case 0xE0:
		LD_acc_data();
		break;

	case 0xEA:
		LD_address_r(registers.acc);
		break;

	case 0xFA:
		LD_r_address(registers.acc);
		break;

	// RST
	case 0xFF:
		RST(0x38);
		break;

	default:
		std::cout << "Unknown opcode: " << std::hex << static_cast<int>(opcode) << std::endl;
		break;
	}
	
	if (branchTaken) {
		instTime = branchedCycles[opcode];
	}
	else {
		instTime = branchlessCycles[opcode];
	}
	clockTime += instTime;
	gpu.step(instTime);
}

void CPU::loadRom(std::string name)
{
	mmu.load(name);
}

void CPU::setByteRegisterVal(byte dst, byte val) {
	if (dst == 6) {
		mmu.writeByte(registers.hl, val);
	}
	else {
		byte* dst_reg = byteRegisters[dst & 0b111];
		*dst_reg = val;
	}
}

void CPU::setByteRegisters(byte src, byte dst) {
	byte src_value;
	if (src == 6) { // (HL)
		src_value = mmu.readByte(registers.hl);
	}
	else {
		src_value = *byteRegisters[src];
	}
	if (dst == 6) {
		mmu.writeByte(registers.hl, src_value);
	}
	else {
		byte* dst_reg = byteRegisters[dst & 0b111];
		*dst_reg = src_value;
	}
	
}

byte CPU::getByteRegister(byte reg_num)
{
	if (reg_num == 6) { // (HL)
		return mmu.readByte(registers.hl);
	}
		
	return *byteRegisters[reg_num];
}

word CPU::getWordAtPC()
{
	byte low = mmu.readByte(registers.pc++);
	byte high = mmu.readByte(registers.pc++);
	return join_bytes(high, low);
}

void CPU::stackPush(word reg)
{

	mmu.writeByte(--registers.sp, (reg >> 8) & 0xFF);
	mmu.writeByte(--registers.sp, reg & 0xFF);
}

void CPU::stackPop(word& reg)
{
	byte low = mmu.readByte(registers.sp++);
	byte high = mmu.readByte(registers.sp++);
	word pop = join_bytes(high, low);
	reg = pop;
}

void CPU::INC_DEC_reg8(byte opcode) // TODO: set flags
{
	byte reg = (opcode >> 3) & 0b111;
	byte type = opcode & 0b1;
	// LSB is 0 if INC, 1 if DEC
	if (type == 0) {
		setByteRegisterVal(reg, getByteRegister(reg) + 1);
	} else {
		setByteRegisterVal(reg, getByteRegister(reg) - 1);
	}
	SET_BIT(registers.flag, ZERO_FLAG, getByteRegister(reg) == 0);
}

// TODO: Set Flags
void CPU::INC_DEC_reg16(byte opcode)
{
	// if the third bit is 0, INC, else DEC
	byte type = ACCESS_BIT(opcode, 3);
	byte reg_num = (opcode >> 4) & 0b11;
	word * reg = &registers.bc;
	switch (reg_num) {
	case 0:
		reg = &registers.bc;
		break;
	case 1:
		reg = &registers.de;
		break;
	case 2:
		reg = &registers.hl;
		break;
	case 3:
		reg = &registers.sp;
		break;
	}
	std::cout << "Modifying register " << (int)reg_num << " with operation " << (int)type << std::endl;
	if (type == 0) {
		(*reg)++;
	} else {
		(*reg)--;
	}

	SET_BIT(registers.flag, ZERO_FLAG, *reg == 0);
}

void CPU::LD_r8(byte opcode)
{
	byte reg = (opcode >> 3) & 0b111;
	byte value = mmu.readByte(registers.pc++);
	setByteRegisterVal(reg, value);
}

void CPU::LD_r16(byte opcode)
{
	byte reg_num = (opcode >> 4) & 0b11;
	word* reg = &registers.bc;
	switch (reg_num) {
	case 0:
		reg = &registers.bc;
		break;
	case 1:
		reg = &registers.de;
		break;
	case 2:
		reg = &registers.hl;
		break;
	case 3:
		reg = &registers.sp;
		break;
	}

	word val = getWordAtPC();
	*reg = val;

}

void CPU::LD_acc_address(byte opcode)
{
	byte reg_num = (opcode >> 4) & 0b11;
	word reg = registers.bc;
	switch (reg_num) {
	case 0:
		reg = registers.bc;
		break;
	case 1:
		reg = registers.de;
		break;
	case 2: case 3:
		reg = registers.hl;
		break;
	}

	registers.acc = mmu.readByte(reg);
	if (reg_num == 2) {
		registers.hl += 1;
	} else if (reg_num == 3) {
		registers.hl -= 1;
	}
}

void CPU::LD_address_acc(byte opcode)
{
	byte reg_num = (opcode >> 4) & 0b11;
	word reg = registers.bc;
	switch (reg_num) {
	case 0:
		reg = registers.bc;
		break;
	case 1:
		reg = registers.de;
		break;
	case 2: case 3:
		reg = registers.hl;
		break;
	}

	mmu.writeByte(reg, registers.acc);
	if (reg_num == 2) {
		registers.hl += 1;
	}
	else if (reg_num == 3) {
		registers.hl -= 1;
	}
}

void CPU::LD_acc_data() {
	byte offset = mmu.readByte(registers.pc++);
	word data_address = 0xFF00 + offset;
	mmu.writeByte(data_address, registers.acc);
}

// Loads the contents of a register into another register
void CPU::LD_r_r(byte opcode)
{
	byte dst = (opcode >> 3) & 0b111;
	byte src = opcode & 0b111;
	setByteRegisters(src, dst);
}

void CPU::LD_address_r(byte reg)
{
	word add = getWordAtPC();
	mmu.writeByte(add, reg);
}

void CPU::LD_r_address(byte& reg) {
	word add = getWordAtPC();
	reg = mmu.readByte(add);
}

void CPU::ALU_r(byte opcode)
{
	byte operation = (opcode >> 3) & 0b111; // grab the ALU opcode
	byte reg = getByteRegister(opcode & 0b111);

	// TODO: set flags
	switch (operation) {
	case 0:
		registers.acc += reg;
		SET_BIT(registers.flag, SUB_FLAG, 0);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 1:
		registers.acc += reg + ACCESS_BIT(registers.flag, CARRY_FLAG);
		SET_BIT(registers.flag, SUB_FLAG, 0);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 2:
		registers.acc -= reg;
		SET_BIT(registers.flag, SUB_FLAG, 1);
		break;
	case 3:
		registers.acc -= reg + ACCESS_BIT(registers.flag, CARRY_FLAG);
		SET_BIT(registers.flag, SUB_FLAG, 1);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 4:
		registers.acc &= (reg);
		SET_BIT(registers.flag, CARRY_FLAG, 0);
		SET_BIT(registers.flag, HALF_CARRY_FLAG, 1);
		SET_BIT(registers.flag, SUB_FLAG, 0);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 5:
		registers.acc ^= (reg);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 6:
		registers.acc |= (reg);
		SET_BIT(registers.flag, ZERO_FLAG, registers.acc == 0);
		break;
	case 7:
		(registers.acc - reg) == 0 ? SET_BIT(registers.flag, ZERO_FLAG, 1) : SET_BIT(registers.flag, ZERO_FLAG, 0);
		break;
	}
}

void CPU::JR() {
	signed_byte offset = static_cast<signed_byte>(mmu.readByte(registers.pc++));
	registers.pc += offset;
}

void CPU::JR(bool flag) {
	signed_byte offset = static_cast<signed_byte>(mmu.readByte(registers.pc++));
	if (flag) {
		registers.pc += offset;
		branchTaken = true;
	}
}

void CPU::JP_16()
{
	word jmp = getWordAtPC();
	registers.pc = jmp;
}

void CPU::RET()
{
	stackPop(registers.pc);
}

void CPU::CALL_16()
{
	word jmp = getWordAtPC();
	stackPush(jmp);
	registers.pc = jmp;
}

void CPU::RST(word address)
{
	stackPush(address);
	registers.pc = address;
}
