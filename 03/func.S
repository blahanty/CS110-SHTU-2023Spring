# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS

LPS:
li a2 '|' # constant separator
li a3 0 # string length
addi sp sp -4
sw ra 0(sp)
jal read_string
li t0 0 # initialize current length of palindromic sub-string
li t1 1 # initialize current maximum length of palindromic sub-string
li a4 2
mul a4 a4 a3
addi a4 a4 1 # after inserting separators, the length of string with length n is 2n + 1
addi a5 a4 -4 # only need to check 2n - 3 characters in the middle
add a5 a5 sp
addi a5 a5 2 # locate to the last separator where check starts whose index is 2n - 2
add a6 a4 sp # right bound (excluded) for t2
addi a7 sp 2 # left check bound (included) for a5
jal check1
add sp sp a4
lw ra 0(sp)
addi sp sp 4
mv a0 t1
jr ra

read_end:
jr ra

read_string:
addi sp sp -1 # allocate memory for separator
sb a2 0(sp) # store separator
lb t0 0(a0) # read 1 bit of string 
beq t0 x0 read_end # if string ends
addi sp sp -1 # allocate memory for the character
sb t0 0(sp) # store the character to stack
addi a3 a3 1 # ++ string length
addi a0 a0 1 # ++ pointer
j read_string # loop

update_max:
mv t1 t0
li t0 0
j check1

char_diff:
blt t1 t0 update_max # update maximum length of palindromic sub-string if necessary
li t0 0 # reinitialize the counter
j check1

finish:
jr ra

check1:
addi a5 a5 -1 # next character
blt a5 a7 finish # all characters have been checked
mv t2 a5
mv t3 a5

check2:
addi t2 t2 -1 # left side index to be checked
addi t3 t3 1 # right index to be checked
blt t2 sp char_diff # reach left bound
bge t3 a6 char_diff # reach right bound
lb t4 0(t2) # left-side character
lb t5 0(t3) # right-side character
bne t4 t5 char_diff # two characters from both sides are different
addi t0 t0 1 # update current length of palindromic sub-string
j check2
