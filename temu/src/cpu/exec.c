#include "helper.h"
#include "all-instr.h"

// 函数指针
typedef void (*op_fun)(uint32_t);
static make_helper(_2byte_esc);

Operands ops_decoded;
uint32_t instr;
	

/* TODO: Add more instructions!!! */

op_fun opcode_table [64] = {
/* 0x00 */	_2byte_esc, inv, inv, inv,
/* 0x04 */	beq, bne, inv, inv,
/* 0x08 */	addi, addiu, slti, sltiu,
/* 0x0c */	andi, ori, inv, lui,
/* 0x10 */	inv, inv, temu_trap, inv,
/* 0x14 */	xori, inv, inv, inv,
/* 0x18 */	inv, inv, inv, inv,
/* 0x1c */	inv, inv, inv, inv,
/* 0x20 */	lb, lh, inv, lw,
/* 0x24 */	lbu, lhu, inv, inv,
/* 0x28 */	sb, sh, inv, sw,
/* 0x2c */	inv, inv, inv, inv,
/* 0x30 */	inv, inv, inv, inv,
/* 0x34 */	inv, inv, inv, inv,
/* 0x38 */	inv, inv, inv, inv,
/* 0x3c */	inv, inv, inv, inv
};

op_fun _2byte_opcode_table[64] = {
	/* 0x00 */ sll, inv, srl, sra,
	/* 0x04 */ sllv, inv, srlv, srav,
	/* 0x08 */ jr, inv, inv, inv,
	/* 0x0c */ inv, inv, inv, inv,
	/* 0x10 */ mfhi, mthi, mflo, mtlo,
	/* 0x14 */ inv, inv, inv, inv,
	/* 0x18 */ mult, multu, div, inv,
	/* 0x1c */ inv, inv, inv, inv,
	/* 0x20 */ add, addu, sub, subu,
	/* 0x24 */ and, or, xor, nor,
	/* 0x28 */ inv, inv, slt, sltu,
	/* 0x2c */ inv, inv, inv, inv,
	/* 0x30 */ inv, inv, inv, inv,
	/* 0x34 */ inv, inv, inv, inv,
	/* 0x38 */ inv, inv, inv, inv,
	/* 0x3c */ inv, inv, inv, inv};

// I-type
make_helper(exec) {
	instr = instr_fetch(pc, 4);
	ops_decoded.opcode = instr >> 26;
	opcode_table[ ops_decoded.opcode ](pc);
}

// R-type
static make_helper(_2byte_esc) {
	ops_decoded.func = instr & FUNC_MASK;
	_2byte_opcode_table[ops_decoded.func](pc); 
}
