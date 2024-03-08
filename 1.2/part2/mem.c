#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem.h"


/*******************************
 * Helper Functions
 *******************************/

/* Call this function in the following cases:
        1. Attempts to access a memory location that it is not allowed to access.
        2. Attempts to access a memory location in a way that is not allowed.
 */
static void raise_segmentation_fault(Address addr) {
    write_to_log("Error - segmentation fault at address %08x\n", addr);
}

/* Call this function if a data access is performed to an address 
   that is not aligned for the size of the access.
 */
static void raise_alignment_fault(Address addr) {
    write_to_log("Error - alignment fault at address %08x\n", addr);
}

/* This function is helpful to your alignment check.
 */
int check_alignment(Address address, Alignment alignment) {
    if (alignment == LENGTH_BYTE) {
        return 1;
    }
        /* HALF_WORD: The last bit is not used. */
    else if (alignment == LENGTH_HALF_WORD) {
        return address % 2 == 0;
    }
        /* WORD: The last two bits are not used. */
    else if (alignment == LENGTH_WORD) {
        return address % 4 == 0;
    } else {
        return 0;
    }
}

/* To store the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually store to memory.
 */

Address map_addr(Address addr) {
    if (addr >= STACK_BASE - (MEMORY_SIZE - MEMORY_STACK) && addr < STACK_BASE &&
        addr + MEMORY_SIZE - STACK_BASE >= MEMORY_STACK)
        /* stack */
        return addr + MEMORY_SIZE - STACK_BASE;
    else if (addr >= DATA_BASE && addr - DATA_BASE < MEMORY_STACK - MEMORY_DATA)
        /* data */
        return addr - DATA_BASE + MEMORY_DATA;
    else if (addr < MEMORY_DATA)
        /* text */
        return addr;
    /* error */
    return 0x114514;
}

void store(Byte *memory, Address address, Alignment alignment, Word value, int check_align) {
    size_t i;
    /* perform an address conversion */
    Address new_addr = map_addr(address);
    /* check segmentation fault */
    if (new_addr < MEMORY_DATA || new_addr + alignment > MEMORY_SIZE ||
        (new_addr < MEMORY_STACK && new_addr + alignment > MEMORY_STACK)) {
        raise_segmentation_fault(address);
        return;
    }
    /* check alignment fault */
    if (check_align == 1 && !check_alignment(new_addr, alignment)) {
        raise_alignment_fault(new_addr);
        return;
    }
    /* store */
    for (i = 0; i < alignment; ++i)
        memory[new_addr + i] = (value >> (i * 8)) & 0xFF;
}

/* To load the data in the corresponding address in memory, 
   please note the following points:
        1. When check_align==1, check if the memory address is aligned
         with the data type. You need to use raise_alignment_fault().
        2. Think about when you need to use raise_segmentation_fault().
        3. Please perform an address conversion before actually load from memory.
 */
Word load(Byte *memory, Address address, Alignment alignment, int check_align) {
    /* check segmentation fault */
    Address new_addr = map_addr(address);
    if ((new_addr < MEMORY_DATA && new_addr + alignment > MEMORY_DATA) ||
        (new_addr >= MEMORY_DATA && new_addr < MEMORY_STACK && new_addr + alignment > MEMORY_STACK) ||
        (new_addr >= MEMORY_STACK && new_addr < MEMORY_SIZE && new_addr + alignment > MEMORY_SIZE) ||
        new_addr >= MEMORY_SIZE) {
        raise_segmentation_fault(address);
        return 0;
    }
    /* check alignment fault */
    if (check_align == 1 && !check_alignment(new_addr, alignment)) {
        raise_alignment_fault(new_addr);
        return 0;
    }
    /* load */
    if (alignment == LENGTH_BYTE)
        /* lb */
        return (Word) *((Byte *) (memory + new_addr));
    else if (alignment == LENGTH_HALF_WORD)
        /* lh */
        return (Word) *((Half *) (memory + new_addr));
    else if (alignment == LENGTH_WORD)
        /* lw */
        return (Word) *((Word *) (memory + new_addr));
    else
        return 0;
}