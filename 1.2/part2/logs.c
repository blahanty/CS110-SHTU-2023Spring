#include <stdlib.h>
#include <string.h>
#include "logs.h"
#include "execute_utils.h"


/*******************************
 * Helper Functions
 *******************************/

/* Creates a new empty Logs and returns a pointer to that Logs. 
   If memory allocation fails, you should call allocation_failed().
 */
Logs *create_logs() {
    /* allocate memory */
    Logs *new_log = (Logs *) malloc(sizeof(Logs));
    /* allocation fails */
    if (!new_log) {
        allocation_failed();
        return NULL;
    }
    /* initialization */
    new_log->size = 0;
    new_log->cap = 8;
    /* allocate memory */
    new_log->modified = (Mod *) calloc(new_log->cap, sizeof(Mod));
    /* allocation fails */
    if (!new_log->modified) {
        free(new_log);
        allocation_failed();
        return NULL;
    }
    /* allocate memory */
    new_log->pc = (Register *) calloc(new_log->cap, sizeof(Register));
    /* allocation fails */
    if (!new_log->pc) {
        free(new_log->modified);
        free(new_log);
        allocation_failed();
        return NULL;
    }
    return new_log;
}

/* Frees the given Logs and all associated memory. */
void free_logs(Logs *logs) {
    size_t i;
    if (logs) {
        /* free memory recorded in log */
        if (logs->modified) {
            for (i = 0; i < logs->size; ++i)
                if (logs->modified[i].mem)
                    free(logs->modified[i].mem);
            free(logs->modified);
        }
        /* free PC */
        if (logs->pc)
            free(logs->pc);
        free(logs);
    }
}

/* Record the current status and save in a logs component.
   Think about what information needs to be recorded.
   Logs can resize to fit an arbitrary number of entries. 
   (so you should use dynamic memory allocation).
 */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs) {
    size_t i;
    Address new_addr;
    /* early error handling on invalid arguments */
    if (cpu && mem && logs) {
        /* double capacity if necessary */
        if (logs->size + 1 >= logs->cap) {
            logs->cap *= 2;
            /* reallocate memory */
            logs->modified = (Mod *) realloc(logs->modified, logs->cap * sizeof(Mod));
            /* allocation fails */
            if (!logs->modified) {
                free_logs(logs);
                allocation_failed();
                return;
            }
            /* reallocate memory */
            logs->pc = (Register *) realloc(logs->pc, logs->cap * sizeof(Register));
            /* allocation fails */
            if (!logs->pc) {
                free_logs(logs);
                allocation_failed();
                return;
            }
        }
        /* modified register initialization */
        logs->modified[logs->size].reg.index = 0;
        logs->modified[logs->size].reg.content = 0;
        /* modified memory initialization */
        logs->modified[logs->size].mem_num = 0;
        logs->modified[logs->size].mem = NULL;
        /* current PC */
        logs->pc[logs->size] = cpu->PC;
        switch (inst.opcode.opcode) {
            case 0x33:
                /* R-format */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.rtype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.rtype.rd];
                break;
            case 0x13:
                /* I-format except ecall, load and jalr */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.itype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.itype.rd];
                break;
            case 0x73:
                /* ecall */
                /* nothing will be modified */
                break;
            case 0x63:
                /* SB-format */
                /* nothing will be modified */
                break;
            case 0x03:
                /* load */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.itype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.itype.rd];
                break;
            case 0x23:
                /* S-format */
                /* memory will be modified */
                if (inst.stype.funct3 == 0x0)
                    /* sb */
                    logs->modified[logs->size].mem_num = LENGTH_BYTE;
                else if (inst.stype.funct3 == 0x1)
                    /* sh */
                    logs->modified[logs->size].mem_num = LENGTH_HALF_WORD;
                else if (inst.stype.funct3 == 0x2)
                    /* sw */
                    logs->modified[logs->size].mem_num = LENGTH_WORD;
                else
                    /* undefined funct3 */
                    logs->modified[logs->size].mem_num = 0;
                new_addr = map_addr(cpu->reg[inst.stype.rs1] + get_store_offset(inst));
                /* valid funct3 */
                if (logs->modified[logs->size].mem_num && new_addr >= MEMORY_DATA &&
                    new_addr + logs->modified[logs->size].mem_num <= MEMORY_SIZE) {
                    /* allocate memory */
                    logs->modified[logs->size].mem = (Piece *) calloc(logs->modified[logs->size].mem_num,
                                                                      sizeof(Piece));
                    /* allocation fails */
                    if (!logs->modified[logs->size].mem) {
                        free_logs(logs);
                        allocation_failed();
                        return;
                    }
                    /* write memory log */
                    for (i = 0; i < logs->modified[logs->size].mem_num; ++i) {
                        logs->modified[logs->size].mem[i].index = new_addr + i;
                        logs->modified[logs->size].mem[i].content = mem[new_addr + i];
                    }
                } else {
                    /* undefined funct3 or invalid address */
                    /* nothing will be modified */
                    logs->modified[logs->size].mem_num = 0;
                    logs->modified[logs->size].mem = NULL;
                }
                break;
            case 0x6F:
                /* UJ-format */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.ujtype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.ujtype.rd];
                break;
            case 0x67:
                /* jalr */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.itype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.itype.rd];
                break;
            case 0x17:
            case 0x37:
                /* U-format */
                /* rd will be modified */
                logs->modified[logs->size].reg.index = inst.utype.rd;
                logs->modified[logs->size].reg.content = cpu->reg[inst.utype.rd];
                break;
            default:
                /* undefined opcode */
                /* nothing will be modified */
                break;
        }
        ++logs->size;
    }
}

/* Back to the previous state recorded in a logs component.
   if success, return 0.
   if fail, for example, no previous state, return -1.
 */
int undo(Processor *cpu, Byte *mem, Logs *logs) {
    size_t i;
    /* early error handling on invalid arguments */
    if (!cpu || !mem || !logs)
        return -1;
    /* no previous state */
    if (!logs->size)
        return -1;
    --logs->size;
    /* recover rd */
    cpu->reg[logs->modified[logs->size].reg.index] = logs->modified[logs->size].reg.content;
    /* recover memory */
    for (i = 0; i < logs->modified[logs->size].mem_num; ++i)
        mem[logs->modified[logs->size].mem[i].index] = logs->modified[logs->size].mem[i].content;
    /* free allocated memory */
    if (logs->modified[logs->size].mem)
        free(logs->modified[logs->size].mem);
    /* recover PC */
    cpu->PC = logs->pc[logs->size];
    return 0;
}

/* Add function definition here if you need */