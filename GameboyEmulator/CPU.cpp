#include "CPU.h"
#include <iostream>

CPU::CPU()
{
	registers = { 0 };
	registers.pc = 0x100;
	
	mmu = std::unique_ptr<MMU>(new MMU());
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


void CPU::step()
{
	byte opcode = mmu->readByte(registers.pc++);

	switch (opcode) {
	case 0x00:
		// nop
		break;

	// 0b01rrrRRR
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f: case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f: case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f: case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
		LD_r_r(opcode);
		break;
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f: case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa7: case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf: case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb7: case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		ALU_r(opcode);
		break;
	default:
		std::cout << std::hex << static_cast<int>(opcode) << std::endl;
		break;
	}
	
}

void CPU::loadRom(std::string name)
{
	mmu->load(name);
}

byte* CPU::getByteRegister(byte num) {
	return byteRegisters[num & 0b111];
}

void CPU::LD_r16(byte opcode)
{

}

// Loads the contents of a register into another register
void CPU::LD_r_r(byte opcode)
{
	byte* r1 = getByteRegister(opcode >> 3);
	byte* r2 = getByteRegister(opcode);
	(*r1) = *r2;
}

void CPU::ALU_r(byte opcode)
{
	byte operation = (opcode >> 3) & 0b111; // grab the ALU opcode
	byte reg = *getByteRegister(opcode);

	// TODO: set flags
	switch (operation) {
	case 0:
		registers.acc += reg;
		break;
	case 1:
		registers.acc += reg + ACCESS_BIT(registers.flag, CARRY_FLAG);
		break;
	case 2:
		registers.acc -= reg;
		break;
	case 3:
		registers.acc -= reg - ACCESS_BIT(registers.flag, CARRY_FLAG);
		break;
	case 4:
		registers.acc &= (reg);
		break;
	case 5:
		registers.acc ^= (reg);
		break;
	case 6:
		registers.acc |= (reg);
		break;
	case 7:
		(registers.acc - reg) == 0 ? SET_BIT(registers.flag, ZERO_FLAG, 1) : SET_BIT(registers.flag, ZERO_FLAG, 0);
		break;
	}
}
