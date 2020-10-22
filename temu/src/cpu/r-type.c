#include "helper.h"
#include "monitor.h"
#include "reg.h"

extern uint32_t instr;
extern char assembly[80];

/* decode R-type instrucion */
static void decode_r_type(uint32_t instr) {

	op_src1->type = OP_TYPE_REG;
	op_src1->reg = (instr & RS_MASK) >> (RT_SIZE + IMM_SIZE);
	op_src1->val = reg_w(op_src1->reg);
	
	op_src2->type = OP_TYPE_REG;
	op_src2->imm = (instr & RT_MASK) >> (RD_SIZE + SHAMT_SIZE + FUNC_SIZE);
	op_src2->val = reg_w(op_src2->reg);

	op_dest->type = OP_TYPE_REG;
	op_dest->reg = (instr & RD_MASK) >> (SHAMT_SIZE + FUNC_SIZE);

	op_sham->type = OP_TYPE_IMM;
	op_sham->imm = instr & SHAMT_MASK;
	op_sham->val = op_sham->imm;
}

make_helper(sll) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val) << (op_sham->val);
	sprintf(assembly, "sll   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_sham->val);
}

make_helper(jr) {

	decode_r_type(instr);
	cpu.pc = op_src1->val;
	sprintf(assembly, "jr   %s", REG_NAME(op_src1->reg));
}

make_helper(mfhi) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = cpu.hi;
	sprintf(assembly, "mfhi   %s", REG_NAME(op_dest->reg));
}

make_helper(mflo) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = cpu.lo;
	sprintf(assembly, "mflo   %s", REG_NAME(op_dest->reg));
}

make_helper(add) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val + op_src2->val);
	sprintf(assembly, "add   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(subu) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val - op_src2->val);
	sprintf(assembly, "subu   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(slt) {

	decode_r_type(instr);
	// signed numbers compare
	int32_t rs = op_src1->val, rt = op_src2->val;
	reg_w(op_dest->reg) = (rs < rt) ? 1 : 0;
	sprintf(assembly, "slt   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

// TODO:carefully
make_helper(mult) {

	decode_r_type(instr);
	// signed numbers multiple
	int64_t rs = op_src1->val, rt = op_src2->val, ans;
	ans = rs * rt;
	cpu.lo = ans;
	cpu.hi = ans >> 32;
	sprintf(assembly, "mult   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(div) {

	decode_r_type(instr);
	// signed div
	int32_t rs = op_src1->val, rt = op_src2->val;
	cpu.lo = rs / rt;
	cpu.hi = rs % rt;
	sprintf(assembly, "div   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(and) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val & op_src2->val);
	sprintf(assembly, "and   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(or) {
	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val | op_src2->val);
	sprintf(assembly, "or   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(xor) {
	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val ^ op_src2->val);
	sprintf(assembly, "xor   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(nor) {
	decode_r_type(instr);
	reg_w(op_dest->reg) = ~(op_src1->val | op_src2->val);
	sprintf(assembly, "nor   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}