#include <stdio.h>
#include "execute.h"
#include "execute_utils.h"
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Function handle with error machine code. You should skip the error 
   machine code after call this function
*/
void raise_machine_code_error(Instruction instruction) {
    write_to_log("Invalid Machine Code: 0x%08x\n", instruction.bits);
}

/* Function handle with undefined ecall parameter.
*/
void raise_undefined_ecall_error(Register a0) {
    write_to_log("Undefined Ecall Number: %d\n", a0);
}

/* Function to exit current program */
void exit_program(Processor *p) {
    if (p->stat == 1) {
        /* set cpu state unwork */
        p->stat = 0;
        write_to_log("Exited with error code 0.\n");
    } else {
        write_to_log("Error: program already exit.\n");
    }
}

/* 
    In this section you need to complete execute instruction. This should modify 
    the appropriate registers, make any necessary calls to memory, and update the program
    counter to refer You will find the functions in execute_utils.h very useful. So it's 
    better to finish execute_utils.c first. The following provides you with a general 
    framework. You can follow this framework or add your own functions, but please 
    don't change declaration or feature of execute(). Case execute() is the entry point
    to the execution phase. The execute_*() is specific implementations for each 
    instruction type. You had better read types.h carefully. Think about the usage 
    of the union defined in types.h
 */

void execute(Instruction inst, Processor *p, Byte *memory) {
    switch (inst.opcode.opcode) {
        case 0x33:
            /* R-format */
            execute_rtype(inst, p);
            break;
        case 0x13:
            /* I-format except ecall, load and jalr */
            execute_itype_except_load(inst, p);
            break;
        case 0x73:
            if (inst.itype.funct3 == 0x0 && !get_imm_operand(inst))
                /* ecall */
                execute_ecall(p, memory);
            else {
                /* undefined imm */
                raise_machine_code_error(inst);
                p->PC += 4;
            }
            break;
        case 0x63:
            /* SB-format */
            execute_branch(inst, p);
            break;
        case 0x03:
            /* load */
            execute_load(inst, p, memory);
            break;
        case 0x23:
            /* S-format */
            execute_store(inst, p, memory);
            break;
        case 0x6F:
            /* UJ-format */
            execute_jal(inst, p);
            break;
        case 0x67:
            /* jalr */
            execute_jalr(inst, p);
            break;
        case 0x17:
        case 0x37:
            /* U-format */
            execute_utype(inst, p);
            break;
        default:
            /* undefined opcode */
            raise_machine_code_error(inst);
            p->PC += 4;
            break;
    }
}

void execute_rtype(Instruction inst, Processor *p) {
    switch (inst.rtype.funct3) {
        /* funct3 */
        case 0x0:
            if (inst.rtype.funct7 == 0x00)
                /* add */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] + p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x01)
                /* mul */
                p->reg[inst.rtype.rd] = (sWord) p->reg[inst.rtype.rs1] * (sWord) p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x20)
                /* sub */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] - p->reg[inst.rtype.rs2];
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x1:
            if (inst.rtype.funct7 == 0x00)
                /* sll */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] << p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x01)
                /* mulh */
                p->reg[inst.rtype.rd] =
                        ((sDouble) (sWord) p->reg[inst.rtype.rs1] * (sDouble) (sWord) p->reg[inst.rtype.rs2]) >> 32;
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x2:
            if (inst.rtype.funct7 == 0x00)
                /* slt */
                p->reg[inst.rtype.rd] = (sWord) p->reg[inst.rtype.rs1] < (sWord) p->reg[inst.rtype.rs2] ? 1 : 0;
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x3:
            if (inst.rtype.funct7 == 0x00)
                /* sltu */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] < p->reg[inst.rtype.rs2] ? 1 : 0;
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x4:
            if (inst.rtype.funct7 == 0x00)
                /* xor */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] ^ p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x01)
                /* div */
                p->reg[inst.rtype.rd] = (sWord) p->reg[inst.rtype.rs1] / (sWord) p->reg[inst.rtype.rs2];
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x5:
            if (inst.rtype.funct7 == 0x00)
                /* srl */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] >> p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x20)
                /* sra */
                p->reg[inst.rtype.rd] = (sWord) p->reg[inst.rtype.rs1] >> p->reg[inst.rtype.rs2];
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x6:
            if (inst.rtype.funct7 == 0x00)
                /* or */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] | p->reg[inst.rtype.rs2];
            else if (inst.rtype.funct7 == 0x01)
                /* rem */
                p->reg[inst.rtype.rd] = (sWord) p->reg[inst.rtype.rs1] % (sWord) p->reg[inst.rtype.rs2];
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x7:
            if (inst.rtype.funct7 == 0x00)
                /* and */
                p->reg[inst.rtype.rd] = p->reg[inst.rtype.rs1] & p->reg[inst.rtype.rs2];
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        default:
            /* undefined funct3 */
            raise_machine_code_error(inst);
            break;
    }
    /* update PC */
    p->PC += 4;
}

void execute_itype_except_load(Instruction inst, Processor *p) {
    switch (inst.itype.funct3) {
        /* funct3 */
        case 0x0:
            /* addi */
            p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] + get_imm_operand(inst);
            break;
        case 0x1:
            if (inst.rtype.funct7 == 0x00)
                /* slli */
                p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] << get_imm_operand(inst);
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x2:
            /* slti */
            p->reg[inst.itype.rd] = (sWord) p->reg[inst.itype.rs1] < get_imm_operand(inst) ? 1 : 0;
            break;
        case 0x3:
            /* sltiu */
            p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] < (Word) get_imm_operand(inst) ? 1 : 0;
            break;
        case 0x4:
            /* xori */
            p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] ^ get_imm_operand(inst);
            break;
        case 0x5:
            if (inst.rtype.funct7 == 0x00)
                /* srli */
                p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] >> (Word) (get_imm_operand(inst) & 0x1F);
            else if (inst.rtype.funct7 == 0x20)
                /* srai */
                p->reg[inst.itype.rd] = (sWord) (p->reg[inst.itype.rs1]) >> (Word) (get_imm_operand(inst) & 0x1F);
            else
                /* undefined funct7 */
                raise_machine_code_error(inst);
            break;
        case 0x6:
            /* ori */
            p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] | get_imm_operand(inst);
            break;
        case 0x7:
            /* andi */
            p->reg[inst.itype.rd] = p->reg[inst.itype.rs1] & get_imm_operand(inst);
            break;
        default:
            /* undefined funct3 */
            raise_machine_code_error(inst);
            break;
    }
    /* update PC */
    p->PC += 4;
}

/* You may need to use exit_program() */
void execute_ecall(Processor *p, Byte *memory) {
    switch (p->reg[10]) {
        /* a0 */
        case 1:
            /* print value of a1 as an integer */
            write_to_log("%d", p->reg[11]);
            break;
        case 4:
            /* print the string at address a1 */
            if (map_addr(p->reg[11]) < MEMORY_SIZE)
                write_to_log("%s", (char *) (memory + map_addr(p->reg[11])));
            break;
        case 10:
            /* exit of end of code indicator */
            exit_program(p);
            break;
        case 11:
            /* print value of a1 as a character */
            write_to_log("%c", p->reg[11]);
            break;
        default:
            /* undefined a0 */
            raise_undefined_ecall_error(p->reg[10]);
            break;
    }
    /* update PC */
    p->PC += 4;
}

void execute_branch(Instruction inst, Processor *p) {
    int offset;
    switch (inst.sbtype.funct3) {
        /* funct3 */
        case 0x0:
            /* beq */
            offset = p->reg[inst.sbtype.rs1] == p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        case 0x1:
            /* bne */
            offset = p->reg[inst.sbtype.rs1] != p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        case 0x4:
            /* blt */
            offset = (sWord) p->reg[inst.sbtype.rs1] < (sWord) p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        case 0x5:
            /* bge */
            offset = (sWord) p->reg[inst.sbtype.rs1] >= (sWord) p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        case 0x6:
            /* bltu */
            offset = p->reg[inst.sbtype.rs1] < p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        case 0x7:
            /* bgeu */
            offset = p->reg[inst.sbtype.rs1] >= p->reg[inst.sbtype.rs2] ? get_branch_offset(inst) : 4;
            /* update PC */
            p->PC += offset;
            break;
        default:
            /* undefined funct3 */
            raise_machine_code_error(inst);
            p->PC += 4;
            break;
    }
}

void execute_load(Instruction inst, Processor *p, Byte *memory) {
    switch (inst.itype.funct3) {
        case 0x0:
            /* lb */
            p->reg[inst.itype.rd] = bitSigner(
                    load(memory, p->reg[inst.itype.rs1] + get_imm_operand(inst), LENGTH_BYTE, 1), 8);
            break;
        case 0x1:
            /* lh */
            p->reg[inst.itype.rd] = bitSigner(
                    load(memory, p->reg[inst.itype.rs1] + get_imm_operand(inst), LENGTH_HALF_WORD, 1), 16);
            break;
        case 0x2:
            /* lw */
            p->reg[inst.itype.rd] =  load(memory, p->reg[inst.itype.rs1] + get_imm_operand(inst), LENGTH_WORD, 1);
            break;
        case 0x4:
            /* lbu */
            p->reg[inst.itype.rd] = load(memory, p->reg[inst.itype.rs1] + get_imm_operand(inst), LENGTH_BYTE, 1);
            break;
        case 0x5:
            /* lhu */
            p->reg[inst.itype.rd] = load(memory, p->reg[inst.itype.rs1] + get_imm_operand(inst), LENGTH_HALF_WORD, 1);
            break;
        default:
            /* undefined funct3 */
            raise_machine_code_error(inst);
            break;
    }
    /* update PC */
    p->PC += 4;
}

void execute_store(Instruction inst, Processor *p, Byte *memory) {
    switch (inst.stype.funct3) {
        case 0x0:
            /* sb */
            store(memory, p->reg[inst.stype.rs1] + get_store_offset(inst), LENGTH_BYTE, p->reg[inst.stype.rs2], 1);
            break;
        case 0x1:
            /* sh */
            store(memory, p->reg[inst.stype.rs1] + get_store_offset(inst), LENGTH_HALF_WORD, p->reg[inst.stype.rs2], 1);
            break;
        case 0x2:
            /* sw */
            store(memory, p->reg[inst.stype.rs1] + get_store_offset(inst), LENGTH_WORD, p->reg[inst.stype.rs2], 1);
            break;
        default:
            /* undefined funct3 */
            raise_machine_code_error(inst);
            break;
    }
    /* update PC */
    p->PC += 4;
}

void execute_jal(Instruction inst, Processor *p) {
    p->reg[inst.ujtype.rd] = p->PC + 4;
    /* update PC */
    p->PC += get_jump_offset(inst);
}

void execute_jalr(Instruction inst, Processor *p) {
    p->reg[inst.itype.rd] = p->PC + 4;
    /* update PC */
    p->PC = p->reg[inst.itype.rs1] + get_imm_operand(inst);
}

void execute_utype(Instruction inst, Processor *p) {
    switch (inst.opcode.opcode) {
        case 0x17:
            /* auipc */
            p->reg[inst.utype.rd] = p->PC + (inst.utype.imm << 12);
            break;
        case 0x37:
            /* lui */
            p->reg[inst.utype.rd] = inst.utype.imm << 12;
            break;
        default:
            break;
    }
    /* update PC */
    p->PC += 4;
}