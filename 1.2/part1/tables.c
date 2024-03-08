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

const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;

/*******************************
 * Helper Functions
 *******************************/

/* incorrect address alignment */
void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n");
}

/* name already exists */
void name_already_exists(const char *name) {
    write_to_log("Error: name '%s' already exists in table.\n", name);
}

/* name does not exist */
void name_does_not_exist(const char *name) {
    write_to_log("Error: name '%s' does not exist in table.\n", name);
}

void write_sym(FILE *output, uint32_t addr, const char *name) {
    fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containing 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will
   need to store this value for use during add_to_table().
 */
SymbolTable *create_table(int mode) {
    /* allocate memory for symbol table */
    SymbolTable *new_table = (SymbolTable *) malloc(sizeof(SymbolTable));
    /* fail to allocate memory */
    if (!new_table) {
        allocation_failed();
        return NULL;
    }
    /* allocate memory for symbols in symbol table with default capacity 16 */
    new_table->tbl = (Symbol *) calloc(16, sizeof(Symbol));
    /* fail to allocate memory */
    if (!new_table->tbl) {
        free(new_table);
        allocation_failed();
        return NULL;
    }
    /* initialization */
    new_table->len = 0;
    new_table->cap = 16;
    new_table->mode = mode;
    return new_table;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable *table) {
    /* initialization */
    uint32_t i;
    /* early error handling on invalid arguments */
    if (table) {
        /* free all associated memory */
        for (i = 0; i < table->len; ++i)
            free(table->tbl[i].name);
        free(table->tbl);
        /* free the given symbol table */
        free(table);
    }
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   1. ADDR is given as the byte offset from the first instruction. 
   2. The SymbolTable must be able to resize itself as more elements are added. 

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned and check_align==1, you should call 
   addr_alignment_incorrect() and return -1. 

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. 

   6.If memory allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0. 
 */

int add_to_table(SymbolTable *table, const char *name, uint32_t addr, int check_align) {
    /* initialization */
    uint32_t i;
    Symbol *new_symbol;
    /* early error handling on invalid arguments */
    if (!table || !name)
        return -1;
    /* check align */
    if (check_align == 1 && (addr % 4)) {
        addr_alignment_incorrect();
        return -1;
    }
    /* check duplicate name */
    if (table->mode == SYMBOLTBL_UNIQUE_NAME)
        for (i = 0; i < table->len; ++i)
            if (!strcmp(table->tbl[i].name, name)) {
                name_already_exists(name);
                return -1;
            }
    /* double the capacity if necessary */
    if (table->len == table->cap) {
        table->tbl = realloc(table->tbl, table->cap * 2 * sizeof(Symbol));
        if (!table->tbl) {
            free_table(table);
            allocation_failed();
            return -1;
        }
        table->cap *= 2;
    }
    /* add to table */
    new_symbol = &table->tbl[table->len];
    /* reallocate memory for symbols */
    new_symbol->name = (char *) malloc(strlen(name) + 1);
    /* fail to reallocate memory */
    if (!new_symbol->name) {
        free_table(table);
        allocation_failed();
        return -1;
    }
    /* setup symbol */
    strcpy(new_symbol->name, name);
    new_symbol->addr = addr;
    /* modify length of symbol table */
    ++table->len;
    return 1;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable *table, const char *name) {
    uint32_t i;
    /* early error handling on invalid arguments */
    if (!table || !name)
        return -1;
    /* look for symbol */
    for (i = 0; i < table->len; ++i)
        if (!strcmp(table->tbl[i].name, name))
            return table->tbl[i].addr;
    /* name does not exist */
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable *table, FILE *output) {
    uint32_t i;
    /* early error handling on invalid arguments */
    if (table && output)
        /* write */
        for (i = 0; i < table->len; ++i)
            write_sym(output, table->tbl[i].addr, table->tbl[i].name);
}

/* DO NOT MODIFY THIS LINE BELOW */
/* ===================================================================== */
/* Read the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable *create_table_from_file(int mode, FILE *file) {
    SymbolTable *symtbl = create_table(mode);
    char buf[1024];
    while (fgets(buf, sizeof(buf), file)) {
        char *name, *addr_str, *end_ptr;
        uint32_t addr;
        addr_str = strtok(buf, "\t");
        /* create table based on these tokens */
        addr = strtol(addr_str, &end_ptr, 0);
        name = strtok(NULL, "\n");
        /* add the name to table, one by one */
        add_to_table(symtbl, name, addr, 0);
    }
    return symtbl;
}