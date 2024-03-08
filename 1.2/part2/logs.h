#ifndef LOGS_H
#define LOGS_H

#include "emulator.h"
#include "mem.h"


/* Complete the following definition of Logs and implement the following
   functions. You are free to add structs or functions, but you must keep
   structure Logs. And you cannot change a function that has already been given.
 */

/* Add structure definition here if you need */

typedef struct {
    /* index and content of register or memory to be modified */
    size_t index;
    Word content;
} Piece;

typedef struct {
    /* rd of the instruction if to be modified */
    Piece reg;
    /* number of bytes and contents of memory to be modified for S-format */
    size_t mem_num;
    Piece *mem;
} Mod;

typedef struct {
    /* all modified things */
    Mod *modified;
    /* size and capacity of log */
    size_t size, cap;
    /* PC of instructions */
    Register *pc;
} Logs;

/* IMPLEMENT ME - see documentation in logs.c */
Logs *create_logs();

/* IMPLEMENT ME - see documentation in logs.c */
void free_logs(Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
void record(Instruction inst, Processor *cpu, Byte *mem, Logs *logs);

/* IMPLEMENT ME - see documentation in logs.c */
int undo(Processor *cpu, Byte *mem, Logs *logs);

/* Add function declaration here if you need */

#endif