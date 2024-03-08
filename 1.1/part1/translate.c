/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tables.h"
#include "translate.h"
#include "translate_utils.h"

/* Writes data during the assembler's first pass to OUTPUT. 
   The input Byte* buf should point to a buffer with a size larger than 8 Bytes.
   Then this function takes the first 4 bytes as a word and write to output FILE.
   And move the second 4 bytes to the head and set second 4 bytes to 0.
   Think about how to use this function in the output data.

   Notes: You can leave this function out, but you need to make sure that 
   the form of the output data file remains the same.
 */

int write_static_data(FILE *output, Byte *buf) {
    fprintf(output, "%08x\n", *((Word *) buf));
    memcpy(buf, buf + 4, 4);
    memset(buf + 4, 0, 4);
    return 0;
}

/* Writes original code during the assembler's first pass to OUTPUT. 
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Returns the number of instructions written (so 0 if there were any errors).

   Notes: Think about what you should do with this return value.
 */

unsigned write_original_code(FILE *output, const char *name, char **args,
                             int num_args) {
    long imm;
    unsigned num_basic_code = 0;
    if (!output || !name || !args)
        return 0;
    /* if-else ladder for original code output */
    if (strcmp(name, "la") == 0) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        if (num_args == 2)
            num_basic_code = 2;
    } else if (strcmp(name, "lw") == 0) {
        /* lw can take two or three parameters */
        if (num_args != 2 && num_args != 3)
            return 0;
        /* lw x11, label */
        if (num_args == 2)
            num_basic_code = 2;
        /* lw x11, 0(x12) */
        if (num_args == 3)
            num_basic_code = 1;
    } else if (strcmp(name, "li") == 0) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        /* translate and check input immediate */
        if (translate_num_32(&imm, args[1]) == -1)
            return 0;
        /* check how many lines are used */
        if (_INT12_MIN_ <= imm && imm <= _INT12_MAX_)
            num_basic_code = 1;
        else
            num_basic_code = 2;
    } else {
        num_basic_code = 1;
    }
    /* perform write string to output here */
    write_inst_string(output, name, args, num_args);
    if (num_basic_code > 1)
        fprintf(output, "-\n");
    return num_basic_code;
}

/* Translate instructions perform a pseudoinstruction expansion and call
   write_instruction() to write the corresponding instruction. The case for
   general instructions, beqz and la has already been completed, but you need to 
   write code to translate the bnez, j, jr, li, lw and mv pseudoinstructions. 
   Your pseudoinstruction expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in write_instruction(). 
   However, for pseudoinstructions, you must make sure that ARGS contains the 
   correct number of arguments. You do NOT need to check whether the registers 
   or label are valid, since that will be checked in write_instruction().

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addi instruction. Otherwise, expand it into
        a lui-addi pair.

   And for other pseudoinstruction:
    - your expansion should be as same as venus. Try them in venus and see what
        will happen. 

   venus has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if venus behaves differently. 

   If writing multiple instructions, make sure that each instruction has correct 
   addrress.

   Returns the number of instructions written (so 0 if there were any errors).
 */


int translate_inst(FILE *basic_code, FILE *machine_code, const char *name, char **args,
                   size_t num_args, uint32_t addr, SymbolTable *symtbl) {
    char *sub_args[3];
    char buf[100];
    long imm, head, uiSec, addSec;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args)
        return 0;
    if (!strcmp(name, "beqz")) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        /* setup parameters */
        sub_args[0] = args[0];
        sub_args[1] = "x0";
        sub_args[2] = args[1];
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "beq", sub_args, 3, addr, symtbl) == -1)
            return 0;
        return 1;
    } else if (!strcmp(name, "bnez")) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        /* setup parameters */
        sub_args[0] = args[0];
        sub_args[1] = "x0";
        sub_args[2] = args[1];
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "bne", sub_args, 3, addr, symtbl) == -1)
            return 0;
        return 1;
    } else if (!strcmp(name, "j")) {
        /* check count of arguments */
        if (num_args != 1)
            return 0;
        /* setup parameters */
        sub_args[0] = "x0";
        sub_args[1] = args[0];
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "jal", sub_args, 2, addr, symtbl) == -1)
            return 0;
        return 1;
    } else if (!strcmp(name, "jr")) {
        /* check count of arguments */
        if (num_args != 1)
            return 0;
        /* setup parameters */
        sub_args[0] = "x0";
        sub_args[1] = args[0];
        sub_args[2] = "0";
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "jalr", sub_args, 3, addr, symtbl) == -1)
            return 0;
        return 1;
    }   /* jal and jalr can be explicitly specified or use default ra */
    else if (!strcmp(name, "jal")) {
        /* setup parameters */
        if (num_args == 1) {
            /* need translation */
            sub_args[0] = "x1";
            sub_args[1] = args[0];
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "jal", sub_args, 2, addr, symtbl) == -1)
                return 0;
            return 1;
        } else if (num_args == 2) {
            /* do not need translation */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "jal", args, 2, addr, symtbl) == -1)
                return 0;
            return 1;
        } else
            /* check count of arguments */
            return 0;
    } else if (!strcmp(name, "jalr")) {
        /* setup parameters */
        if (num_args == 1) {
            /* need translation */
            sub_args[0] = "x1";
            sub_args[1] = args[0];
            sub_args[2] = "0";
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "jalr", sub_args, 3, addr, symtbl) == -1)
                return 0;
            return 1;
        } else if (num_args == 3) {
            /* do not need translation */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "jalr", args, 3, addr, symtbl) == -1)
                return 0;
            return 1;
        } else
            /* check count of arguments */
            return 0;
    } else if (!strcmp(name, "la")) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        /* get the symbol address */
        imm = get_addr_for_symbol(symtbl, args[1]);
        if (imm == -1)
            return 0;
        /* get the symbol offset from current PC */
        imm -= addr;
        /* setup parameters for auipc */
        head = (imm + 0x800) >> 12;
        sprintf(buf, "%d", STATIC_CAST(int, head));
        sub_args[0] = args[0];
        sub_args[1] = buf;
        /* auipc */
        if (write_instruction(basic_code, machine_code, "auipc", sub_args, 2, addr, symtbl) == -1)
            return 0;
        /* setup parameters for addi */
        imm = imm - (head << 12);
        sprintf(buf, "%d", STATIC_CAST(int, imm));
        sub_args[0] = args[0];
        sub_args[1] = args[0];
        sub_args[2] = buf;
        /* addi */
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr + 4, symtbl) == -1)
            return 0;
        return 2;
    } else if (!strcmp(name, "lw")) {
        if (num_args == 2) {
            /* get the symbol address */
            imm = get_addr_for_symbol(symtbl, args[1]);
            if (imm == -1)
                return 0;
            /* get the symbol offset from current PC */
            imm -= addr;
            /* setup parameters for auipc */
            head = (imm + 0x800) >> 12;
            sprintf(buf, "%d", STATIC_CAST(int, head));
            sub_args[0] = args[0];
            sub_args[1] = buf;
            /* auipc */
            if (write_instruction(basic_code, machine_code, "auipc", sub_args, 2, addr, symtbl) == -1)
                return 0;
            /* setup parameters for lw */
            imm -= head << 12;
            sprintf(buf, "%d", STATIC_CAST(int, imm));
            sub_args[0] = args[0];
            sub_args[1] = buf;
            sub_args[2] = args[0];
            /* lw */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "lw", sub_args, 3, addr + 4, symtbl) == -1)
                return 0;
            return 2;
        } else if (num_args == 3) {
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "lw", args, 3, addr, symtbl) == -1)
                return 0;
            return 1;
        } else
            /* check count of arguments */
            return 0;
    } else if (!strcmp(name, "li")) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        if (translate_num_32(&imm, args[1]) == -1)
            return 0;
        if (translate_num_12(&imm, args[1]) == -1) {
            /* have to be handled by lui and addi */
            /* setup parameters for lui */
            uiSec = (imm + 0x800) >> 12;
            sprintf(buf, "%d", STATIC_CAST(int, uiSec));
            sub_args[0] = args[0];
            sub_args[1] = buf;
            /* lui */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "lui", sub_args, 2, addr, symtbl) == -1)
                return 0;
            /* setup parameters for addi */
            addSec = imm - (uiSec << 12);
            sprintf(buf, "%d", STATIC_CAST(int, addSec));
            sub_args[0] = args[0];
            sub_args[1] = args[0];
            sub_args[2] = buf;
            /* addi */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr + 4, symtbl) == -1)
                return 0;
            return 2;
        } else {
            /* can be handled by addi only */
            sub_args[0] = args[0];
            sub_args[1] = "x0";
            sub_args[2] = args[1];
            /* addi */
            /* error if not successful */
            if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr, symtbl) == -1)
                return 0;
            return 1;
        }
    } else if (!strcmp(name, "mv")) {
        /* check count of arguments */
        if (num_args != 2)
            return 0;
        /* setup parameters */
        sub_args[0] = args[0];
        sub_args[1] = args[1];
        sub_args[2] = "0";
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, "addi", sub_args, 3, addr, symtbl) == -1)
            return 0;
        return 1;
    } else {
        /* handle non-pseudo ones */
        /* error if not successful */
        if (write_instruction(basic_code, machine_code, name, args, num_args, addr, symtbl) == -1)
            return 0;
        return 1;
    }
    return 0;
}

/* Writes the instruction in basic and hexadecimal format to OUTPUT during 
   pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The address (addr) is given for current instruction PC address.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. venus may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error.
 */


int write_instruction(FILE *basic_code, FILE *machine_code, const char *name, char **args,
                      size_t num_args, uint32_t addr, SymbolTable *symtbl) {
    /** R-format */
    /* add */
    if (strcmp(name, "add") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x00, args, num_args);
        /* mul */
    else if (strcmp(name, "mul") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x01, args, num_args);
        /* sub */
    else if (strcmp(name, "sub") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x00, 0x20, args, num_args);
        /* sll */
    else if (strcmp(name, "sll") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x01, 0x00, args, num_args);
        /* mulh */
    else if (strcmp(name, "mulh") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x01, 0x01, args, num_args);
        /* slt */
    else if (strcmp(name, "slt") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x02, 0x00, args, num_args);
        /* sltu */
    else if (strcmp(name, "sltu") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x03, 0x00, args, num_args);
        /* xor */
    else if (strcmp(name, "xor") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x04, 0x00, args, num_args);
        /* div */
    else if (strcmp(name, "div") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x04, 0x01, args, num_args);
        /* srl */
    else if (strcmp(name, "srl") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x05, 0x00, args, num_args);
        /* sra */
    else if (strcmp(name, "sra") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x05, 0x20, args, num_args);
        /* or */
    else if (strcmp(name, "or") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x06, 0x00, args, num_args);
        /* rem */
    else if (strcmp(name, "rem") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x06, 0x01, args, num_args);
        /* and */
    else if (strcmp(name, "and") == 0)
        return write_rtype(basic_code, machine_code, name, 0x33, 0x07, 0x00, args, num_args);
    /** I-format */
    /* lb */
    if (!strcmp(name, "lb"))
        return write_itype(basic_code, machine_code, name, 0x03, 0x00, args, num_args);
        /* lh */
    else if (!strcmp(name, "lh"))
        return write_itype(basic_code, machine_code, name, 0x03, 0x01, args, num_args);
        /* lw */
    else if (!strcmp(name, "lw"))
        return write_itype(basic_code, machine_code, name, 0x03, 0x02, args, num_args);
        /* lbu */
    else if (!strcmp(name, "lbu"))
        return write_itype(basic_code, machine_code, name, 0x03, 0x04, args, num_args);
        /* lhu*/
    else if (!strcmp(name, "lhu"))
        return write_itype(basic_code, machine_code, name, 0x03, 0x05, args, num_args);
        /* addi */
    else if (!strcmp(name, "addi"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x00, args, num_args);
        /* slli */
    else if (!strcmp(name, "slli"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x01, args, num_args);
        /* slti */
    else if (!strcmp(name, "slti"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x02, args, num_args);
        /* sltiu */
    else if (!strcmp(name, "sltiu"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x03, args, num_args);
        /* xori */
    else if (!strcmp(name, "xori"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x04, args, num_args);
        /* srli */
    else if (!strcmp(name, "srli"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x05, args, num_args);
        /* srai */
    else if (!strcmp(name, "srai"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x05, args, num_args);
        /* ori */
    else if (!strcmp(name, "ori"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x06, args, num_args);
        /* andi */
    else if (!strcmp(name, "andi"))
        return write_itype(basic_code, machine_code, name, 0x13, 0x07, args, num_args);
        /* jalr */
    else if (!strcmp(name, "jalr"))
        return write_itype(basic_code, machine_code, name, 0x67, 0x00, args, num_args);
        /* ecall */
    else if (!strcmp(name, "ecall"))
        return write_itype(basic_code, machine_code, name, 0x73, 0x00, args, num_args);
    /** S-format */
    /* sb */
    if (!strcmp(name, "sb"))
        return write_stype(basic_code, machine_code, name, 0x23, 0x00, args, num_args);
        /* sh */
    else if (!strcmp(name, "sh"))
        return write_stype(basic_code, machine_code, name, 0x23, 0x01, args, num_args);
        /* sw */
    else if (!strcmp(name, "sw"))
        return write_stype(basic_code, machine_code, name, 0x23, 0x02, args, num_args);
    /** SB-format */
    /* beq */
    if (!strcmp(name, "beq"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x00, args, num_args, addr, symtbl);
        /* bne */
    else if (!strcmp(name, "bne"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x01, args, num_args, addr, symtbl);
        /* blt */
    else if (!strcmp(name, "blt"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x04, args, num_args, addr, symtbl);
        /* bge */
    else if (!strcmp(name, "bge"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x05, args, num_args, addr, symtbl);
        /* bltu */
    else if (!strcmp(name, "bltu"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x06, args, num_args, addr, symtbl);
        /* bgeu */
    else if (!strcmp(name, "bgeu"))
        return write_sbtype(basic_code, machine_code, name, 0x63, 0x07, args, num_args, addr, symtbl);
    /** U-format */
    /* auipc */
    if (!strcmp(name, "auipc"))
        return write_utype(basic_code, machine_code, name, 0x17, args, num_args);
        /* lui */
    else if (!strcmp(name, "lui"))
        return write_utype(basic_code, machine_code, name, 0x37, args, num_args);
    /** UJ-format */
    /* jal */
    if (!strcmp(name, "jal"))
        return write_ujtype(basic_code, machine_code, name, 0x6F, args, num_args, addr, symtbl);
    else
        return -1;
}


/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_rtype(), write_inst_stype(),
   write_inst_sbtype(), write_inst_utype(), write_inst_ecall(), write_inst_hex() 
   to write to basic_code or machine_code. Both are defined in translate_utils.h.

   Function write_rtype() is complete for reference. Complete other functions
   implementation below. You will find bitwise operations to be the cleanest 
   way to complete this function.

   Returns 0 on success and -1 on failure
 */
int write_rtype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                uint8_t funct3, uint8_t funct7, char **args, size_t num_args) {
    /* R-format requires rd rs rt */
    int rd, rs, rt;
    uint32_t instruction;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args)
        return -1;
    /* check count of arguments */
    if (num_args != 3)
        return -1;
    /* destination register */
    rd = translate_reg(args[0]);
    /* source register 1 */
    rs = translate_reg(args[1]);
    /* source register 2 */
    rt = translate_reg(args[2]);
    /* error checking for register ids */
    if (rd == -1 || rs == -1 || rt == -1)
        return -1;
    /* write basic code */
    write_inst_rtype(basic_code, name, rd, rs, rt);
    /* generate instruction */
    instruction = opcode + (rd << 7) + (funct3 << 12) + (rs << 15) + (rt << 20) + (funct7 << 25);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}

int write_itype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                uint8_t funct3, char **args, size_t num_args) {
    /* I-format requires rd offset(rs) or rd rs imm */
    int rd, rs;
    long int imm;
    uint32_t instruction;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args)
        return -1;
    /* ecall */
    if (!strcmp(name, "ecall")) {
        /* write basic code */
        write_inst_ecall(basic_code, name);
        /* instruction == opcode */
        /* write machine code */
        write_inst_hex(machine_code, opcode);
        return 0;
    }
    /* check count of arguments */
    if (num_args != 3)
        return -1;
    /* destination register */
    rd = translate_reg(args[0]);
    if (opcode == 0x03) {
        /* rd offset(rs) */
        /* source register */
        rs = translate_reg(args[2]);
        /* offset */
        /* error checking for register ids and offset */
        if (rd == -1 || rs == -1 || translate_num_12(&imm, args[1]) == -1)
            return -1;
        /* write basic code */
        write_inst_stype(basic_code, name, rd, rs, STATIC_CAST(int, imm));
    } else {
        /* rd rs imm */
        /* source register */
        rs = translate_reg(args[1]);
        /* immediate */
        /* error checking for register ids and immediate */
        if (rd == -1 || rs == -1 || translate_num_12(&imm, args[2]) == -1)
            return -1;
        /* write basic code */
        write_inst_sbtype(basic_code, name, rd, rs, STATIC_CAST(int, imm));
    }
    /* generate instruction */
    if (!strcmp(name, "srai"))
        /* srai has non-zero func7 or imm */
        instruction = opcode + (rd << 7) + (funct3 << 12) + (rs << 15) + (imm << 20) + (0x20 << 25);
    else
        instruction = opcode + (rd << 7) + (funct3 << 12) + (rs << 15) + (imm << 20);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}

int write_stype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                uint8_t funct3, char **args, size_t num_args) {
    /* S-format requires rs rt imm */
    int rs, rt;
    long int imm;
    uint32_t instruction;
    int imm1, imm2;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args)
        return -1;
    /* check count of arguments */
    if (num_args != 3)
        return -1;
    /* source register 1 */
    rs = translate_reg(args[2]);
    /* source register 2 */
    rt = translate_reg(args[0]);
    /* error checking for register ids and immediate */
    /* immediate */
    if (rs == -1 || rt == -1 || translate_num_12(&imm, args[1]))
        return -1;
    /* partition immediate */
    imm1 = imm & 0x1F;
    imm2 = imm >> 5;
    /* write basic code */
    write_inst_stype(basic_code, name, rt, rs, STATIC_CAST(int, imm));
    /* generate instruction */
    instruction = opcode + (imm1 << 7) + (funct3 << 12) + (rs << 15) + (rt << 20) + (imm2 << 25);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}

/* Hint: the way for branch to calculate relative address. 
   e.g. bne bne rs rt label
   assume the byte_offset(addr) of label is L,
   current instruction byte_offset(addr) is A
   the relative address I for label satisfy:
     L = A + I
   so the relative address is
     I = L - A              */
int write_sbtype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                 uint8_t funct3, char **args, size_t num_args, uint32_t addr, SymbolTable *symbol) {
    /* SB-format requires rs rt offset/label */
    int rs, rt;
    long int imm;
    uint32_t instruction;
    int64_t label_address;
    int imm1, imm2, imm3, imm4;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args || !symbol)
        return -1;
    /* check count of arguments */
    if (num_args != 3)
        return -1;
    /* source register 1 */
    rs = translate_reg(args[0]);
    /* source register 2 */
    rt = translate_reg(args[1]);
    /* error checking for register ids */
    if (rs == -1 || rt == -1)
        return -1;
    /* get label address */
    label_address = get_addr_for_symbol(symbol, args[2]);
    /* error checking for label address  */
    if (label_address == -1)
        return -1;
    /* calculate offset */
    imm = label_address - addr;
    /* error checking for offset */
    if (imm < _INT12_MIN_ || imm > _INT12_MAX_)
        return -1;
    /* partition immediate */
    imm1 = imm >> 11 & 1;
    imm2 = imm >> 1 & 0xF;
    imm3 = imm >> 5 & 0x3F;
    imm4 = imm >> 12 & 1;
    /* write basic code */
    write_inst_sbtype(basic_code, name, rs, rt, STATIC_CAST(int, imm));
    /* generate instruction */
    instruction = opcode + (imm1 << 7) + (imm2 << 8) + (funct3 << 12) + (rs << 15) +
                  (rt << 20) + (imm3 << 25) + (imm4 << 31);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}

int write_utype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                char **args, size_t num_args) {
    /* U-format requires rd offset */
    int rd;
    long int imm;
    uint32_t instruction;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args)
        return -1;
    /* check count of arguments */
    if (num_args != 2)
        return -1;
    /* destination register */
    rd = translate_reg(args[0]);
    /* error checking for register ids and offset */
    if (rd == -1 || translate_num(&imm, args[1], 0, _UINT20_MAX_) == -1)
        return -1;
    /* write basic code */
    write_inst_utype(basic_code, name, rd, STATIC_CAST(int, imm));
    /* generate instruction */
    instruction = opcode + (rd << 7) + (imm << 12);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}

/* In this project there is no need to relocate labels,
   you may think about the reasons. */
int write_ujtype(FILE *basic_code, FILE *machine_code, const char *name, uint8_t opcode,
                 char **args, size_t num_args, uint32_t addr, SymbolTable *symbol) {
    /* UJ-format requires rd offset/label */
    int rd;
    uint32_t instruction;
    int64_t label_address;
    long int imm;
    int imm1, imm2, imm3, imm4;
    /* early error handling on invalid arguments */
    if (!basic_code || !machine_code || !name || !args || !symbol)
        return -1;
    /* check count of arguments */
    if (num_args != 2)
        return -1;
    /* destination register */
    rd = translate_reg(args[0]);
    /* error checking for register ids */
    if (rd == -1)
        return -1;
    /* get label address */
    label_address = get_addr_for_symbol(symbol, args[1]);
    if (label_address == -1)
        return -1;
    /* calculate offset */
    imm = label_address - addr;
    /* error checking for offset */
    if (imm < _INT20_MIN_ || imm > _INT20_MAX_)
        return -1;
    /* partition immediate */
    imm1 = imm >> 12 & 0xFF;
    imm2 = imm >> 11 & 1;
    imm3 = imm >> 1 & 0x3FF;
    imm4 = imm >> 20 & 1;
    /* write basic code */
    write_inst_utype(basic_code, name, rd, STATIC_CAST(int, imm));
    /* generate instruction */
    instruction = opcode + (rd << 7) + (imm1 << 12) + (imm2 << 20) + (imm3 << 21) + (imm4 << 31);
    /* write machine code */
    write_inst_hex(machine_code, instruction);
    return 0;
}
