#include "helper.h"
#include "monitor.h"
#include "reg.h"

#include "memory.h"

extern uint32_t instr;
extern char assembly[80];

/* decode I-type instrucion with unsigned immediate */
static void decode_imm_type(uint32_t instr) {

	op_src1->type = OP_TYPE_REG;
	op_src1->reg = (instr & RS_MASK) >> (RT_SIZE + IMM_SIZE);
	op_src1->val = reg_w(op_src1->reg);
	
	op_src2->type = OP_TYPE_IMM;
	op_src2->imm = instr & IMM_MASK;
	// op_src2->simm = op_src2->imm;
	op_src2->val = op_src2->imm;

	op_dest->type = OP_TYPE_REG;
	op_dest->reg = (instr & RT_MASK) >> (IMM_SIZE);	
}

make_helper(beq) {

	decode_imm_type(instr);
	if (op_src1->val == op_dest->val) {
		uint32_t target_offset = (op_src2->simm << 16) >> 14;
		cpu.pc += target_offset;
	}
	sprintf(assembly, "beq   %s,   %s,   0x%04x", REG_NAME(op_src1->reg), REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(bne) {

	decode_imm_type(instr);
	if (op_src1->val != op_dest->val) {
		uint32_t target_offset = (op_src2->simm << 16) >> 14;
		cpu.pc += target_offset;
	}
	sprintf(assembly, "bne   %s,   %s,   0x%04x", REG_NAME(op_src1->reg), REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(addi) {

	decode_imm_type(instr);
	reg_w(op_dest->reg) = (op_src1->val + ((op_src2->simm << 16) >> 16));
	sprintf(assembly, "addi   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(addiu) {

	decode_imm_type(instr);
	reg_w(op_dest->reg) = (op_src1->val + ((op_src2->simm << 16) >> 16));
	sprintf(assembly, "addiu   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(slti) {

	decode_imm_type(instr);
	// signed extension
	int32_t imm = ((op_src2->simm << 16) >> 16);
	// signed num cmp
	int32_t rs = op_src1->val;
	reg_w(op_dest->reg) = rs < imm ? 1 : 0;
	sprintf(assembly, "slti   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(sltiu) {

	decode_imm_type(instr);
	// signed extension
	uint32_t imm = ((op_src2->simm << 16) >> 16);
	// unsigned num cmp
	reg_w(op_dest->reg) = op_src1->val < imm ? 1 : 0;
	sprintf(assembly, "sltiu   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(andi) {

	decode_imm_type(instr);
	// unsigned extension
	uint32_t imm = ((op_src2->val) << 16) >> 16;
	reg_w(op_dest->reg) = op_src1->val & imm;
	sprintf(assembly, "andi   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(lui) {

	decode_imm_type(instr);
	reg_w(op_dest->reg) = (op_src2->val << 16);
	sprintf(assembly, "lui   %s,   0x%04x", REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(ori) {

	decode_imm_type(instr);
	reg_w(op_dest->reg) = op_src1->val | op_src2->val;
	sprintf(assembly, "ori   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(xori) {

	decode_imm_type(instr);
	// unsigned extension
	uint32_t imm = ((op_src2->val) << 16) >> 16;
	reg_w(op_dest->reg) = op_src1->val ^ imm;
	sprintf(assembly, "xori   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

// TODO:expr is right?  0x%04x(%s)
make_helper(lb) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	// read memory
	int32_t ret = mem_read(addr, 1);
	// signed extension
	reg_w(op_dest->reg) = ((ret << 24) >> 24);
	sprintf(assembly, "lb   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

// TODO:
make_helper(lh) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	// read memory
	int32_t ret = mem_read(addr, 2);
	// signed extension
	reg_w(op_dest->reg) = ((ret << 16) >> 16);
	sprintf(assembly, "lh   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

// TODO:
make_helper(lw) {

	decode_imm_type(instr);
	// signed extension
	uint32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	// read memory
	int32_t ret = mem_read(addr, 4);
	// signed extension
	reg_w(op_dest->reg) = ret;
	sprintf(assembly, "lw   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lbu) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	// read memory
	uint32_t ret = mem_read(addr, 1);
	// unsigned extension
	reg_w(op_dest->reg) = ((ret << 24) >> 24);
	sprintf(assembly, "lbu   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lhu) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	// read memory
	uint32_t ret = mem_read(addr, 2);
	// unsigned extension
	reg_w(op_dest->reg) = ((ret << 16) >> 16);
	sprintf(assembly, "lhu   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sb) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	uint8_t ans = reg_w(op_dest->reg);
	// write memory
	mem_write(addr, 1, ans);
	
	sprintf(assembly, "sb   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sh) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	uint8_t ans = reg_w(op_dest->reg);
	// write memory
	mem_write(addr, 2, ans);
	
	sprintf(assembly, "sh   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sw) {

	decode_imm_type(instr);
	// signed extension
	int32_t offset = ((op_src2->simm << 16) >> 16);
	// memory addr
	uint32_t addr = op_src1->val + offset;
	uint32_t ans = reg_w(op_dest->reg);
	// write memory
	mem_write(addr, 4, ans);
	
	sprintf(assembly, "sw   %s,   0x%04x(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}