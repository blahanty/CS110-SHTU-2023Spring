#include <stdio.h>
#include <stdlib.h>
#include "execute_utils.h"


/* You may find implementing this function helpful */

/* Signed extension of a number to 32 bits. 
   size is the number of valid bits of field.
   e.g. field=0x8a, size=8, return 0xFFFFFF8a.
        field=0x7a, size=8, return 0x0000007a.
 */
int bitSigner(unsigned int field, unsigned int size) {
    return field >> (size - 1) ? (0xFFFFFFFF >> size << size) | field : field;
}

/* Return the imm from instruction, for details, please refer to I type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_imm_operand(Instruction instruction) {
    return bitSigner(instruction.itype.imm, 12);
}

/* Return the offset from instruction, for details, please refer to SB type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_branch_offset(Instruction instruction) {
    return bitSigner(((instruction.sbtype.imm5 & 1) << 11) + (((instruction.sbtype.imm5 >> 1) & 0xF) << 1) +
           ((instruction.sbtype.imm7 & 0x3F) << 5) + (((instruction.sbtype.imm7 >> 6) & 1) << 12), 13);
}

/* Return the offset from instruction, for details, please refer to UJ type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_jump_offset(Instruction instruction) {
    return bitSigner(((instruction.ujtype.imm & 0xFF) << 12) + (((instruction.ujtype.imm >> 8) & 1) << 11) +
                     (((instruction.ujtype.imm >> 9) & 0x3FF) << 1) + (((instruction.ujtype.imm >> 19) & 1) << 20), 21);
}

/* Return the offset from instruction, for details, please refer to S type instruction formats.
   Remember that the offsets should return the offset in BYTES.
 */
int get_store_offset(Instruction instruction) {
    return bitSigner(instruction.stype.imm5 + (instruction.stype.imm7 << 5), 12);
}
