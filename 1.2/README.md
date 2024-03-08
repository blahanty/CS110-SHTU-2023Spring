Project 1: Make a Toy Venus
===========================

Project 1.2: A RISC-V Emulator
------------------------------

[Computer Architecture I](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/index.html) [ShanghaiTech University](https://www.shanghaitech.edu.cn/)

  

[Project 1.1](https://toast-lab.sist.shanghaitech.edu.cn/1.1.html) Project 1.2

  

### IMPORTANT INFO - PLEASE READ

The projects are part of your design project worth 2 credit points. As such they run in parallel to the actual course. So be aware that the due date for project and homework might be very close to each other! Start early and do not procrastinate.

Contents
--------

1.  [Goal of Project 1](about:blank#goal)
2.  [Introduction to Project 1.2](about:blank#intro)
3.  [Background of The Instruction Set](about:blank#background)
4.  [Getting Started](about:blank#getting_started) ( [Step 0](about:blank#getting_started_step_0), [Step 1](about:blank#getting_started_step_1), [Step 2](about:blank#getting_started_step_2), [Step 3](about:blank#getting_started_step_3), [Step 4](about:blank#getting_started_step_4) )
5.  [Testing](about:blank#test)
6.  [Running the Emulator](about:blank#run)
7.  [Notices on Grading](about:blank#grade)
8.  [How many lines of code I need to write?](about:blank#expected_workload)
9.  [Submission](about:blank#submission)
10.  [Autolab Results](about:blank#autolab_result)

* * *

Goal of Project 1 [\[<<\]](about:blank#contents) [\[<\]](about:blank#contents) [\[>\]](about:blank#intro)
---------------------------------------------------------------------------------------------------------

In Project 1, you will make a simple toy Venus, consisting of both assembler and emulator. If you are not yet familiar with [Venus](https://autolab.sist.shanghaitech.edu.cn/venus), try it out now. At the end of this project, you will understand the C programming language and the underlying execution of RISC-V assembly.

Introduction to Project 1.2 [\[<<\]](about:blank#contents) [\[<\]](about:blank#goal) [\[>\]](about:blank#background)
--------------------------------------------------------------------------------------------------------------------

In Project 1.1, we have completed an RISC-V assembler. Now we need to complete a simple emulator in C programming language to execute instructions. The emulator can be divided into the following components.

1.  Launching and Running: This part, mainly in emulator.c, is responsible for creating a emulator and performing operations such as run(), step(), prev(), etc. We've done this part for you so you can ignore it!
2.  Execution: This part, mainly in execute.c and execute\_utils.c, is responsible for executing an instruction and finishing the corresponding operations on cpu and memory. You need to work on this component.
3.  Memory: This part, mainly in mem.c, is responsible for simple conversion of memory addresses and manipulation of memory such as store() and load(). You need to work on this component.
4.  Logs component: This part, mainly in logs.c, is responsible for record and save logs to go back in time. You need to work on this component.

Background of The Instruction Set [\[<<\]](about:blank#contents) [\[<\]](about:blank#intro) [\[>\]](about:blank#getting_started)
--------------------------------------------------------------------------------------------------------------------------------

### Registers

Please consult the [RISC-V Green Sheet (PDF)](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/lecture_notes/riscvcard.pdf) for register numbers, instruction opcodes, and bitwise formats. Our asembler will support all 32 registers: **x0**, **ra**, **sp**, **gp**, **tp**, **t0-t6**, **s0 - s11**, **a0 - a7**. The name **x0** can be used in lieu of zero. Other register numbers (eg. x1, x2, etc.) shall be also supported. Note that floating point registers are not included in project 1.

### Instructions

We will have 42 instructions 8 pseudo-instructions. The instructions are:

| Instruction | Type | Opcode | Funct3 | Funct7/IMM | Operation |
| --- | --- | --- | --- | --- | --- |
| add rd, rs1, rs2 | R | 0x33 | 0x0 | 0x00 | R\[rd\] ← R\[rs1\] + R\[rs2\] |
| mul rd, rs1, rs2 | 0x0 | 0x01 | R\[rd\] ← (R\[rs1\] \* R\[rs2\])\[31:0\] |
| sub rd, rs1, rs2 | 0x0 | 0x20 | R\[rd\] ← R\[rs1\] - R\[rs2\] |
| sll rd, rs1, rs2 | 0x1 | 0x00 | R\[rd\] ← R\[rs1\] << R\[rs2\] |
| mulh rd, rs1, rs2 | 0x1 | 0x01 | R\[rd\] ← (R\[rs1\] \* R\[rs2\])\[63:32\] |
| slt rd, rs1, rs2 | 0x2 | 0x00 | R\[rd\] ← (R\[rs1\] < R\[rs2\]) ? 1 : 0 |
| sltu rd, rs1, rs2 | 0x3 | 0x00 | R\[rd\] ← (U(R\[rs1\]) < U(R\[rs2\])) ? 1 : 0 |
| xor rd, rs1, rs2 | 0x4 | 0x00 | R\[rd\] ← R\[rs1\] ^ R\[rs2\] |
| div rd, rs1, rs2 | 0x4 | 0x01 | R\[rd\] ← R\[rs1\] / R\[rs2\] |
| srl rd, rs1, rs2 | 0x5 | 0x00 | R\[rd\] ← R\[rs1\] >> R\[rs2\] |
| sra rd, rs1, rs2 | 0x5 | 0x20 | R\[rd\] ← R\[rs1\] >> R\[rs2\] |
| or rd, rs1, rs2 | 0x6 | 0x00 | R\[rd\] ← R\[rs1\] | R\[rs2\] |
| rem rd, rs1, rs2 | 0x6 | 0x01 | R\[rd\] ← (R\[rs1\] % R\[rs2\] |
| and rd, rs1, rs2 | 0x7 | 0x00 | R\[rd\] ← R\[rs1\] & R\[rs2\] |
| lb rd, offset(rs1) | I | 0x03 | 0x0 |  | R\[rd\] ← SignExt(Mem(R\[rs1\] + offset, byte)) |
| lh rd, offset(rs1) | 0x1 |  | R\[rd\] ← SignExt(Mem(R\[rs1\] + offset, half)) |
| lw rd, offset(rs1) | 0x2 |  | R\[rd\] ← Mem(R\[rs1\] + offset, word) |
| lbu rd, offset(rs1) | 0x4 |  | R\[rd\] ← U(Mem(R\[rs1\] + offset, byte)) |
| lhu rd, offset(rs1) | 0x5 |  | R\[rd\] ← U(Mem(R\[rs1\] + offset, half)) |
| addi rd, rs1, imm | 0x13 | 0x0 |  | R\[rd\] ← R\[rs1\] + imm |
| slli rd, rs1, imm | 0x1 | 0x00 | R\[rd\] ← R\[rs1\] << imm |
| slti rd, rs1, imm | 0x2 |  | R\[rd\] ← (R\[rs1\] < imm) ? 1 : 0 |
| sltiu rd, rs1, imm | 0x3 |  | R\[rd\] ← (U(R\[rs1\]) < U(imm)) ? 1 : 0 |
| xori rd, rs1, imm | 0x4 |  | R\[rd\] ← R\[rs1\] ^ imm |
| srli rd, rs1, imm | 0x5 | 0x00 | R\[rd\] ← R\[rs1\] >> imm |
| srai rd, rs1, imm | 0x5 | 0x20 | R\[rd\] ← R\[rs1\] >> imm |
| ori rd, rs1, imm | 0x6 |  | R\[rd\] ← R\[rs1\] | imm |
| andi rd, rs1, imm | 0x7 |  | R\[rd\] ← R\[rs1\] & imm |
| jalr rd, rs1, imm | 0x67 | 0x0 |  | R\[rd\] ← PC + 4  
PC ← R\[rs1\] + imm |
| ecall | 0x73 | 0x0 | 0x000 | (Transfers control to operating system)<br>a0 = 1 is print value of a1 as an integer.<br>a0 = 4 is print the string at address a1.<br>a0 = 10 is exit or end of code indicator.<br>a0 = 11 is print value of a1 as a character. |
| sb rs2, offset(rs1) | S | 0x23 | 0x0 |  | Mem(R\[rs1\] + offset) ← R\[rs2\]\[7:0\] |
| sh rs2, offset(rs1) | 0x1 |  | Mem(R\[rs1\] + offset) ← R\[rs2\]\[15:0\] |
| sw rs2, offset(rs1) | 0x2 |  | Mem(R\[rs1\] + offset) ← R\[rs2\] |
| beq rs1, rs2, offset | SB | 0x63 | 0x0 |  | if(R\[rs1\] == R\[rs2\])  
 PC ← PC + {offset, 1b'0} |
| bne rs1, rs2, offset | 0x1 |  | if(R\[rs1\] != R\[rs2\])  
 PC ← PC + {offset, 1b'0} |
| blt rs1, rs2, offset | 0x4 |  | if(R\[rs1\] < R\[rs2\])  
 PC ← PC + {offset, 1b'0} |
| bge rs1, rs2, offset | 0x5 |  | if(R\[rs1\] >= R\[rs2\])  
 PC ← PC + {offset, 1b'0} |
| bltu rs1, rs2, offset | 0x6 |  | if(U(R\[rs1\]) < U(R\[rs2\]))  
 PC ← PC + {offset, 1b'0} |
| bgeu rs1, rs2, offset | 0x7 |  | if(U(R\[rs1\]) >= U(R\[rs2\]))  
 PC ← PC + {offset, 1b'0} |
| auipc rd, offset | U | 0x17 |  |  | R\[rd\] ← PC + {offset, 12b'0} |
| lui rd, offset | 0x37 |  |  | R\[rd\] ← {offset, 12b'0} |
| jal rd, imm | UJ | 0x6f |  |  | R\[rd\] ← PC + 4  
PC ← PC + {imm, 1b'0} |

The pseudo-instructions are:

| Pseudo-instruction | Format | Uses |
| --- | --- | --- |
| Branch on Equal to Zero | beqz rs1, label | beq |
| Branch on not Equal to Zero | bnez rs1, label | bne |
| Jump | j label | jal |
| Jump Register | jr rs1 | jalr |
| Load Address | la rd, label | auipc, addi |
| Load Immediate | li rd, immediate | lui, addi |
| Load Word at address of Label | lw rd, label | auipc, lw |
| Move | mv rd, rs1 | addi |

For further reference, here are the bit lengths of the instruction components.

<table><tbody><tr><td>R-TYPE</td><td><span class="inst">funct7</span></td><td><span class="rgtr">rs2</span></td><td><span class="rgtr">rs1</span></td><td><span class="inst">funct3</span></td><td><span class="rgtr">rd</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>7</td><td>5</td><td>5</td><td>3</td><td>5</td><td>7</td></tr></tbody></table>

  

<table><tbody><tr><td>I-TYPE</td><td><span class="immd">imm[11:0]</span></td><td><span class="rgtr">rs1</span></td><td><span class="inst">funct3</span></td><td><span class="rgtr">rd</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>12</td><td>5</td><td>3</td><td>5</td><td>7</td></tr></tbody></table>

  

<table><tbody><tr><td>S-TYPE</td><td><span class="immd">imm[11:5]</span></td><td><span class="rgtr">rs2</span></td><td><span class="rgtr">rs1</span></td><td><span class="inst">funct3</span></td><td><span class="immd">imm[4:0]</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>7</td><td>5</td><td>5</td><td>3</td><td>5</td><td>7</td></tr></tbody></table>

  

<table><tbody><tr><td>SB-TYPE</td><td><span class="immd">imm[12]</span></td><td><span class="immd">imm[10:5]</span></td><td><span class="rgtr">rs2</span></td><td><span class="rgtr">rs1</span></td><td><span class="inst">funct3</span></td><td><span class="immd">imm[4:1]</span></td><td><span class="immd">imm[11]</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>1</td><td>6</td><td>5</td><td>5</td><td>3</td><td>4</td><td>1</td><td>7</td></tr></tbody></table>

  

<table><tbody><tr><td>U-TYPE</td><td><span class="immd">imm[31:12]</span></td><td><span class="rgtr">rd</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>20</td><td>5</td><td>7</td></tr></tbody></table>

  

<table><tbody><tr><td>UJ-TYPE</td><td><span class="immd">imm[20]</span></td><td><span class="immd">imm[10:1]</span></td><td><span class="immd">imm[11]</span></td><td><span class="immd">imm[19:12]</span></td><td><span class="rgtr">rd</span></td><td><span class="inst">opcode</span></td></tr><tr><td>Bits</td><td>1</td><td>10</td><td>1</td><td>8</td><td>5</td><td>7</td></tr></tbody></table>

Getting Started [\[<<\]](about:blank#contents) [\[<\]](about:blank#background) [\[>\]](about:blank#test)
--------------------------------------------------------------------------------------------------------

### Step 0: Obtaining the Files

**Caveat**  
If you want to use the files from Project 1.1, which may help you to generate some testcases, you are recommended to copy previous source files into Project 1.2 working directory. We modified the Makefile for project 1.2. Please be aware of that when importing your Project 1.1 code.

1.  Clone your p1 repository from gitlab. You may want to change http to https.
2.  In the repository add a remote repo that contains the framework files:  
    git remote add framework https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110\_23s/p1.2\_framework.git (or change to http)
3.  Go and fetch the files:  
    git fetch framework
4.  Now merge those files with your main branch:  
    git merge framework/main
5.  The rest of the git commands work as usual.

### Step 1: Building Blocks

Finish the implementation of bitSigner(), get\_imm\_operand(), get\_branch\_offset(), get\_jump\_offset(), get\_store\_offset() in execute\_utils.c.

These functions extract the immediate numbers and offsets contained in the instructions to facilitate subsequent execution. If you have completed project 1.1, you should be familiar with this procedure, as you have done the reverse of it.

### Step 2: Complete Memory Component

You should complete the memory component before the execution component. Finish the implementation of store() and load() in mem.c. You should check the alignment first if needed. check\_alignment() in mem.c could help you.

Before operating on memory, a conversion from virtual memory addresses to real memory addresses needs to be completed. To keep things simple, we will convert a section of space from the start of segment directly to a real section of "physical memory address". You should follow the mapping given below.

**Caveat**  
You have to fix the mapping from virtual DATA\_BASE and TEXT\_BASE to be sequential onto physical memory. Plus, DATA\_BASE and TEXT\_BASE have to be mapped to MEMORY\_DATA and MEMORY\_TEXT, respectively.

![Memory Mapping](https://toast-lab.sist.shanghaitech.edu.cn/courses/CS110@ShanghaiTech/Spring-2023/project/proj1html/memory-mapping.png)

**Caveat**  
Raise segmentation fault when accessing an unmapped memory location and attempting to store in text segment. For an incorrect load you should raise the corresponding error and return a value of 0. For an incorrect store you should raise the corresponding error and return. Do not interrupt or exit the program.

### Step 3: Time to Execute

execute() in execute.c executes the instruction provided as a parameter. This should modify the appropriate registers, make any necessary calls to memory, and update the program counter to refer to the next instruction to execute. You need to complete all the above instructions.

You can use the framework we provide. In that case you will need to complete execute\_rtype(), execute\_itype\_except\_load(), execute\_ecall(), execute\_branch(), execute\_load(), execute\_store(), execute\_jal(), execute\_jalr() and execute\_utype(), or you can implement your own function, but please do not change declaration or feature of execute().

Note that you should use write\_to\_log() when implementing printing function of ecall.

You will find what you have completed in the first two steps very useful in this step.

**Caveat**  
For unrecognised instructions you should use raise\_machine\_code\_error(). For undefined ecalld calls you should use raise\_undefined\_ecall\_error(). During access operations, the alignment is checked according to the corresponding instruction. Raise error when an error occurs and then continue with the next instruction without interrupting or exiting the program.

### Step 4: Don't Forget Logs

In the logs component logs.c, you need to implement a data structure to save the current state and return to the previous state. The main purpose of this component is to implement the prev function of venus. Try the prev function in venus and think about what you would do to implement one of these functions. You need to ensure that the following interfaces are completed.

Logs: You can change its contents or define more structures, but you can't change its name. It represents the log component you created.

*   create\_logs(): Create new logs component, return a pointer to its Logs.
*   free\_logs(): Free a logs component.
*   record(): record the current status in a logs component.
*   undo(): back to the previous state in a logs component.

You should use dynamic memory allocation to resize the logs component when needed. Consider what information you need to record, the smaller the size of each record, the better.

Testing [\[<<\]](about:blank#contents) [\[<\]](about:blank#getting_started) [\[>\]](about:blank#run)
----------------------------------------------------------------------------------------------------

You are responsible for testing your code. While we have provided a few test cases, they are by no means comprehensive. Fortunately, you have a variety of testing tools at your service.

### Valgrind

You should use Valgrind to check whether your code has any memory leaks. We have included a file, run-valgrind, which will run Valgrind on any executable of your choosing. If you get a permission denied error, try changing adding the execute permission to the file:

```
chmod u+x run-valgrind
```

Then you can run by typing:

```
./run-valgrind <whatever program you want to run>
```

### Venus

Since you're writing an emulator, why not refer to an existing one? Venus is a powerful reference for you to use, and you are encouraged to write your own RISC-V files and assemble them using Venus.

**Caveat**  
The initial values of the registers and the initial values of the memory will be somewhat different, this is also related to the version of venus you are using, you can ignore these differences as we have done the initialisation of the memory and registers for you in the project.

### Diff

diff is a utility for comparing the contents of files. Running the following command will print out the differences between file1 and file2:

```
diff <file1> <file2>
```

To see how to interpret diff results, [click here](http://en.wikipedia.org/wiki/Diff_utility#Usage).

Running the Emulator [\[<<\]](about:blank#contents) [\[<\]](about:blank#test) [\[>\]](about:blank#grade)
--------------------------------------------------------------------------------------------------------

First, make sure your emulator executable is up to date by running make.

By default, the program gives help message if you do not specify any arguments.

To run emulator, use the following command.

```
./riscv emulator <text_file> <data_file>
```

If you have a finished part1, you can also use an assembly file as input file like this:

```
./riscv <input file>
```

When emulator is running you can use the following commands to debug.

```
run, step, prev, dump, trace, reset, exit, print <args>.
```

where the <args> in print <args> can be either "pc" "reg" or a memory address

Notices on Grading [\[<<\]](about:blank#contents) [\[<\]](about:blank#run) [\[>\]](about:blank#expected_workload)
-----------------------------------------------------------------------------------------------------------------

*   The Autolab will enforce a proper amount of comments again!
*   Make sure you add proper comments - about two every eight lines of code that you ADD. We will check this by hand.
*   The Autolab will again use \-Wpedantic -Wall -Wextra -Werror -std=c89. You may edit the Makefile if you need to - but the Autolab will replace the CFLAGS line. Your compilation step will have to use the CFLAGS!

How many lines of code I need to write? [\[<<\]](about:blank#contents) [\[<\]](about:blank#grade) [\[>\]](about:blank#submission)
---------------------------------------------------------------------------------------------------------------------------------

Here is a summary of the solution code. The final row gives total lines inserted and deleted; a changed line counts as both an insertion and a deletion. However, there are many possible solutions and many of them may differ.

```

      part2/execute.c       | 492 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      part2/execute_utils.c |  30 +++++++++++++
      part2/logs.c          | 161 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-
      part2/logs.h          |  17 ++++++--
      part2/mem.c           |  73 ++++++++++++++++++++++++++++++++
      5 files changed, 769 insertions(+), 4 deletions(-)
    
```

Submission [\[<<\]](about:blank#contents) [\[<\]](about:blank#expected_workload) [\[>\]](about:blank#autolab_result)
--------------------------------------------------------------------------------------------------------------------

1.  Create a text file autolab.txt. The first line has to be the name of your p1.2 project in gitlab. So p1.2\_email1\_email2.
2.  The following lines have to contain a long, random secret. Commit and push to gitlab. We will test the length and randomness of this secret by running tar -cjf size.tar.bz2 autolab.txt.
3.  When you want to run the autograder in autolab, you have to upload your autolab.txt. Autolab will clone, from gitlab, the default branch (in most case is "main" branch), of the repo specified in the autolab.txt you uploaded and then continue grading only if all of these conditions are met:
    1.  The autolab.txt you uploaded and the one in the clone repo are identical.
    2.  The size of the generated size.tar.bz2 is at least 1000B.
    3.  Only the files from the framework are present in the cloned repo.
4.  Autolab will replace all files except \*.c, \*.h with the framework versions before compiling your code.

Autolab Results [\[<<\]](about:blank#contents) [\[<\]](about:blank#submission)
------------------------------------------------------------------------------

tests 0-xx stands for common results, and the cases are listed below.

| Test Case Number | Point of Interest |
| --- | --- |
| 0-00 | Basic arithmetic operations |
| 0-01 | Instructions involving unsigned numbers |
| 0-02 & 0-03 | Logs component and prev operations |
| 0-04 | ecall |
| 0-05 | branch and jump |
| 0-06 | Error handling |
| 0-07 & 0-08 | Arithmetic involving negative numbers |
| 0-09 ~ 0-14 | Complex |

tests 0-xx-mem stands for memory leak detection test. All common tests are enforced with a memory leak detection test.

* * *

In Project 1 are,

Chundong Wang <`wangchd` AT `shanghaitech.edu.cn`\>  
Siting Liu <`liust` AT `shanghaitech.edu.cn`\>  

and,

Lei Jia <`jialei2022` AT `shanghaitech.edu.cn`\>  
Meng Chen <`chenmeng` AT `shanghaitech.edu.cn`\>  
Haoran Jiang <`jianghr1` AT `shanghaitech.edu.cn`\>  
Linjie Ma <`malj` AT `shanghaitech.edu.cn`\>

  
Based on UC Berkely CS61C and modification for RISC-V by Zhijie Yang  
Last modified: 2023-03-14