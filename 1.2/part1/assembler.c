/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "tables.h"
#include "translate.h"
#include "translate_utils.h"

#define MAX_ARGS 3
#define BUF_SIZE 1024
const char *IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label) {
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line,
                                       const char *extra_arg) {
    write_to_log("Error - extra argument at line %d: %s\n", input_line,
                 extra_arg);
}

/* You should call this function if write_original_code() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name,
                                    char **args, int num_args) {

    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str) {
    char *comment_start = strchr(str, '#');
    if (comment_start) {
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this
   count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,
                        SymbolTable *symtbl, int check_align) {

    size_t len = strlen(str);
    if (str[len - 1] == ':') {
        str[len - 1] = '\0';
        if (is_valid_label(str)) {
            /* add to table and return number of tokens, otherwise return -1 */
            if (add_to_table(symtbl, str, byte_offset, check_align) == 0)
                return 1;
            else
                return -1;
        } else {
            /* we raise error if no valid label is found */
            raise_label_error(input_line, str);
            return -1;
        }
    } else
        return 0;
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_original_code(), pass data to write_static_data(). 
   The symbol table and data segment should also been built and written to 
   specified file. The input file may or may not be valid. Here are some guidelines:

   1. Only one of ".data" or ".text" may be present per line. It must be the 
      first token present. Once you see a ".data" or ".text", the rest part
      will be treated as data or text segment until the next ".data" or ".text"
      Appears. Ignore the rest token of this line. Default to text segment 
      if not indicated.
    
   2. For data segment, we only considering ".word", ".half", ".Byte" types.
      These types must be the first token of a line. The rest of the tokens 
      in this line will be stored as variables of that type in the data segment.
      You can assume that these numbers are legal with their types.

   3. Only one label may be present per line. It must be the first token
      present. Once you see a label, regardless of whether it is a valid label
      or invalid label, You can treat the NEXT token in this line as the 
      beginning of an instruction or a static data.

   4. If the first token is not a label, treat it as the name of an
      instruction. DO NOT try to check it is a valid instruction in this pass.
   
   5. Everything after the instruction name in the same line should be treated 
      as arguments to that instruction. If there are more than MAX_ARGS 
      arguments, call raise_extra_argument_error() and pass in the first extra 
      argument. Do not write that instruction to the original file (e.g. don't call
      write_original_code())
   
   6. Only one instruction should be present per line. You do not need to do
       anything extra to detect this - it should be handled by guideline 5.
   
   8. A line containing only a label is valid. The address of the label should
      be the address of the next instruction or static data regardless of 
      whether there is a next instruction or static data or not.
   
   9. If an instruction contains an immediate, you should output it AS IS.
   
   10. Comments should always be skipped before any further process.

   11. Note that the starting address of the text segment is defined as TEXT_BASE
   in "../types.", and the starting address of the data segment is defined as 
   DATA_BASE in "../types.".

   12. Think about how you should use write_original_code() and write_static_data().
   
   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were
   encountered, it should return 0.
 */

int pass_one(FILE *input, FILE *original, FILE *symtbl, FILE *data) {
    /* initialization */
    /* create symbol table with unique name mode */
    SymbolTable *st = create_table(SYMBOLTBL_UNIQUE_NAME);
    /* current section (0 for .data and 1 for .text), type of data (1 for .byte, 2 for .half, 4 for .word),
     * number of errors, instructions written, temporary return value, index for loop and size of data buffet */
    int section = 1, data_type, error = 0, instruction = 0, ret_value, i = 0, data_buf_size = 0;
    /* line number and PC */
    uint32_t line = 0, pc_text = TEXT_BASE, pc_data = DATA_BASE;
    /* number of arguments */
    size_t args;
    /* token, arguments and the first token which is often instruction name */
    char *token, *argument[MAX_ARGS], *inst_name;
    /* input buffet */
    char buf[BUF_SIZE];
    /* translation result from string to number */
    long int trans_data;
    /* data buffet */
    Byte *data_buf = (Byte *) calloc(BUF_SIZE, sizeof(Byte));
    /* error checking */
    if (!input || !original || !symtbl || !data)
        return -1;
    /* read lines in input */
    while (fgets(buf, BUF_SIZE, input)) {
        /* update line number */
        ++line;
        /* skip comments */
        skip_comments(buf);
        /* empty line */
        if (!strlen(buf))
            continue;
        /* first token */
        inst_name = strtok(buf, IGNORE_CHARS);
        if (!inst_name)
            continue;
        if (!strcmp(inst_name, ".data")) {
            /* .data */
            section = 0;
            /* ignore the rest tokens */
            continue;
        } else if (!strcmp(inst_name, ".text")) {
            /* .text */
            section = 1;
            /* ignore the rest tokens */
            continue;
        }
        if (section) {
            /* .text */
            /* handle label */
            ret_value = add_if_label(line, inst_name, pc_text, st, 1);
            if (ret_value == -1) {
                /* not a valid label */
                ++error;
                /* switch to next token */
                inst_name = strtok(NULL, IGNORE_CHARS);
            } else if (ret_value == 1)
                /* a valid label */
                inst_name = strtok(NULL, IGNORE_CHARS);
            /* check the token */
            if (!inst_name)
                continue;
            /* initialization */
            args = 0;
            token = strtok(NULL, IGNORE_CHARS);
            /* read the arguments */
            while (token) {
                /* check argument number */
                ++args;
                if (args > MAX_ARGS) {
                    /* raise error */
                    raise_extra_argument_error(line, token);
                    ++error;
                    break;
                }
                /* valid token */
                argument[args - 1] = token;
                token = strtok(NULL, IGNORE_CHARS);
            }
            /* invalid argument number */
            if (args > MAX_ARGS)
                continue;
            /* write original code */
            instruction = write_original_code(original, inst_name, argument, args);
            if (!instruction) {
                /* raise error */
                raise_instruction_error(line, inst_name, argument, args);
                ++error;
                continue;
            }
            /* update PC */
            pc_text += instruction * 4;
        } else {
            /* .data */
            /* handle label */
            ret_value = add_if_label(line, inst_name, pc_data, st, 0);
            if (ret_value == -1) {
                /* not a valid label */
                ++error;
                /* switch to next token */
                inst_name = strtok(NULL, IGNORE_CHARS);
            } else if (ret_value == 1)
                /* a valid label */
                inst_name = strtok(NULL, IGNORE_CHARS);
            /* check the token */
            if (!inst_name)
                continue;
            token = inst_name;
            /* type of data */
            if (!strcmp(inst_name, ".byte")) {
                /* .byte: 1 byte */
                data_type = LENGTH_BYTE;
                token = strtok(NULL, IGNORE_CHARS);
            } else if (!strcmp(inst_name, ".half")) {
                /* .half: 2 bytes */
                data_type = LENGTH_HALF_WORD;
                token = strtok(NULL, IGNORE_CHARS);
            } else if (!strcmp(inst_name, ".word")) {
                /* .word: 4 bytes */
                data_type = LENGTH_WORD;
                token = strtok(NULL, IGNORE_CHARS);
            }
            /* put all data into data_buf */
            while (token) {
                /* translate number to long int */
                ret_value = translate_num_32(&trans_data, token);
                /* translate to Byte */
                for (i = 0; i < data_type; ++i)
                    /* get the required byte of number */
                    data_buf[data_buf_size + i] = STATIC_CAST(Byte, (trans_data >> (i * 8)) & 0xFF);
                /* update the size of data buffet */
                data_buf_size += data_type;
                /* update PC */
                pc_data += data_type;
                while (data_buf_size >= 4) {
                    /* write static data in order */
                    ret_value = write_static_data(data, data_buf);
                    data_buf_size -= 4;
                }
                token = strtok(NULL, IGNORE_CHARS);
            }
        }
    }
    /* write the rest bytes */
    if (data_buf_size)
        write_static_data(data, data_buf);
    /* write symbol table */
    write_table(st, symtbl);
    /* free memory */
    free(data_buf);
    free_table(st);
    return error ? -1 : 0;
}

/* Second pass of the assembler.

   This function should read an original file and the corresponding symbol table
   file, translates it into basic code and machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
    7. The line may contain only one "-" which is placeholder you can ignore.
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */


int pass_two(FILE *original, FILE *symtbl, FILE *basic, FILE *machine) {
    /* initialization */
    /* creat symbol table from file */
    SymbolTable *st = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl);
    /* number of errors and written lines */
    int error = 0, line_written;
    /* line number and pc */
    uint32_t line = 0, pc = 0;
    /* number of arguments */
    size_t args;
    /* arguments and the first token which is often instruction name */
    char *argument[MAX_ARGS], *inst_name, *token;
    /* input buffet */
    char buf[BUF_SIZE];
    /* error checking */
    if (!original || !basic || !machine)
        return -1;
    /* read lines in original */
    while (fgets(buf, BUF_SIZE, original)) {
        /* empty line */
        if (!strlen(buf))
            continue;
        /* placeholder */
        if (buf[0] == '-')
            continue;
        /* update line number */
        ++line;
        /* the first token */
        inst_name = strtok(buf, IGNORE_CHARS);
        /* empty line */
        if (!inst_name)
            continue;
        /* initialization */
        args = 0;
        token = strtok(NULL, IGNORE_CHARS);
        /* read the arguments */
        while (token != NULL) {
            argument[args++] = token;
            token = strtok(NULL, IGNORE_CHARS);
        }
        /* count the number of lines written */
        line_written = translate_inst(basic, machine, inst_name, argument, args, pc, st);
        /* error checking and raising */
        if (!line_written) {
            raise_instruction_error(line, inst_name, argument, args);
            ++error;
        }
        /* update PC */
        pc += line_written * 4;
    }
    /* free memory */
    free_table(st);
    return error ? -1 : 0;
}


/*******************************
 * Do Not Modify Code Below
 *******************************/


/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(int mode, FILE *input, FILE *data, FILE *symtbl, FILE *orgin, FILE *basic, FILE *text) {
    int err = 0;

    /* Execute pass one if mode two is not specified */
    if (mode != 2) {
        rewind(input);
        if (pass_one(input, orgin, symtbl, data) != 0) {
            err = 1;
        }
    }

    /* Execute pass two if mode one is not specified */
    if (mode != 1) {
        rewind(orgin);
        rewind(symtbl);
        if (pass_two(orgin, symtbl, basic, text) != 0) {
            err = 1;
        }
    }

    /* Error handling */
    if (err) {
        write_to_log("One or more errors encountered during assembly operation.\n");
    } else {
        write_to_log("Assembly operation completed successfully!\n");
    }

    return err;
}