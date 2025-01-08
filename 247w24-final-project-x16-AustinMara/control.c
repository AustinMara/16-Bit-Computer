#include <stdio.h>
#include <stdlib.h>
#include "bits.h"
#include "control.h"
#include "instruction.h"
#include "x16.h"
#include "trap.h"
#include "decode.h"


// Update condition code based on result
void update_cond(x16_t* machine, reg_t reg) {
    uint16_t result = x16_reg(machine, reg);
    if (result == 0) {
        x16_set(machine, R_COND, FL_ZRO);
    } else if (is_negative(result)) {
        x16_set(machine, R_COND, FL_NEG);
    } else {
        x16_set(machine, R_COND, FL_POS);
    }
}
// Execute a single instruction in the given X16 machine. Update
// memory and registers as required. PC is advanced as appropriate.
// Return 0 on success, or -1 if an error or HALT is encountered.
int execute_instruction(x16_t* machine) {
    // Fetch the instruction and advance the program counter
    uint16_t pc = x16_pc(machine);
    uint16_t instruction = x16_memread(machine, pc);
    x16_set(machine, R_PC, pc + 1);
    if (LOG) {
        fprintf(LOGFP, "0x%x: %s\n", pc, decode(instruction));
    }
    // Variables we might need in various instructions
    reg_t dst, src1, src2, base;
    uint16_t result, indirect, offset, imm, cond, jsrflag, op1, op2, addr;
    // Decode the instruction
    uint16_t opcode = getopcode(instruction);
    switch (opcode) {
        case OP_ADD:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            op1 = x16_reg(machine, src1);
            if (!getbit(instruction, 5)){
                src2 = getbits(instruction, 0, 3);
                op2 = x16_reg(machine, src2);
                result = op1 + op2;
            } else {
                imm = sign_extend(getbits(instruction, 0, 5), 5);
                result = op1 + imm;
            }
            x16_set(machine, dst, result);
            update_cond(machine, dst);
            break;
        case OP_AND:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            op1 = x16_reg(machine, src1);
            if (!getbit(instruction, 5)){
                src2 = getbits(instruction, 0, 3);
                op2 = x16_reg(machine, src2);
                result = op1 & op2;
            } else {
                imm = sign_extend(getbits(instruction, 0, 5), 5);
                result = op1 & imm;
            }
            x16_set(machine, dst, result);
            update_cond(machine, dst);
            break;
        case OP_NOT:
            dst = getbits(instruction, 9, 3);
            src1 = getbits(instruction, 6, 3);
            op1 = x16_reg(machine, src1);
            result = ~op1;
            x16_set(machine, dst, result);
            update_cond(machine, dst);
            break;
        case OP_BR:
            cond = x16_cond(machine);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            uint16_t bit11, bit10, bit9;
            bit11 = getbit(instruction, 11);
            bit10 = getbit(instruction, 10);
            bit9 = getbit(instruction, 9);
            bool n_set, z_set, p_set;
            n_set = ((bit11 == 1 && cond == FL_NEG));
            z_set = ((bit10 == 1 && cond == FL_ZRO));
            p_set = ((bit9 == 1 && cond == FL_POS));
            if (n_set || z_set || p_set){
                x16_set(machine, R_PC, pc + offset + 1);
            } else if (getbits(instruction, 9, 3) == 0x0){
                x16_set(machine, R_PC, pc + offset + 1);
            }
            break;
        case OP_JMP:
            uint16_t val = x16_reg(machine, getbits(instruction, 6, 3));
            x16_set(machine, R_PC, val);
            break;
        case OP_JSR:
            x16_set(machine, R_R7, pc + 1);
            base = getbits(instruction, 6, 3);
            if (getbit(instruction, 11) == 0){
                x16_set(machine, R_PC, x16_reg(machine, base));
            } else {
                offset = sign_extend(getbits(instruction, 0, 11), 11);
                x16_set(machine, R_PC, pc + offset + 1);
            }
            break;
        case OP_LD:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            addr = x16_memread(machine, pc + offset + 1);
            x16_set(machine, dst, addr);
            update_cond(machine, dst);
            break;
        case OP_LDI:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            uint16_t address = x16_memread(machine, pc + offset + 1);
            x16_set(machine, dst, x16_memread(machine, address));
            update_cond(machine, dst);
            break;
        case OP_LDR:
            dst = getbits(instruction, 9, 3);
            base = getbits(instruction, 6, 3);
            offset = sign_extend(getbits(instruction, 0, 6), 6);
            uint16_t reg = x16_reg(machine, base) + offset;
            uint16_t value = x16_memread(machine, reg);
            x16_set(machine, dst, value);
            update_cond(machine, dst);
            break;
        case OP_LEA:
            dst = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            x16_set(machine, dst, pc + offset + 1);
            update_cond(machine, dst);
            break;
        case OP_ST:
            src1 = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            op1 = x16_reg(machine, src1);
            x16_memwrite(machine, pc + offset + 1, op1);
            break;
        case OP_STI:
            src1 = getbits(instruction, 9, 3);
            offset = sign_extend(getbits(instruction, 0, 9), 9);
            op1 = x16_reg(machine, src1);
            addr = x16_memread(machine, pc + offset + 1);
            x16_memwrite(machine, addr, op1);
            break;
        case OP_STR:
            src1 = getbits(instruction, 9, 3);
            base = getbits(instruction, 6, 3);
            offset = sign_extend(getbits(instruction, 0, 6), 6);
            op1 = x16_reg(machine, src1);
            op2 = x16_reg(machine, base);
            x16_memwrite(machine, offset + op2, op1);
            break;
        case OP_TRAP:
            // Execute the trap -- do not rewrite
            return trap(machine, instruction);
        case OP_RES:
        case OP_RTI:
        default:
            // Bad codes, never used
            abort();
    }
    return 0;
}
