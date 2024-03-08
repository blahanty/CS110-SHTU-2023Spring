.data
n: .word 3 # number of test data
data: # input, answer
.word 0x12345678, 0x1E6A2C48
.word 0x71C924BF, 0xFD24938E
.word 0x19260817, 0xE8106498

.text
# === main and helper functions ===
# You don't need to understand these, but reading them may be useful
main:
    la t0, n
    lw s0, 0(t0)
    la s1, data
    main_loop: # run each of the n tests
        beq s0, zero, main_loop_end
        lw a0, 0(s1)
        jal print_hex_and_space # print input
        lw a0, 4(s1)
        jal print_hex_and_space # print answer
        lw a0, 0(s1)
        jal bitrev1
        jal print_hex_and_space # print result of bitrev1
        lw a0, 0(s1)
        jal bitrev2
        jal print_hex_and_space # print result of bitrev2
        jal print_newline
        addi s0, s0, -1
        addi s1, s1, 8
        j main_loop
    main_loop_end:
    li a0, 10
    ecall # exit

print_hex_and_space:
    mv a1, a0
    li a0, 34
    ecall
    li a1, ' '
    li a0, 11
    ecall
    ret

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    ret

# === The first version ===
# Reverse the bits in a0 with a loop
bitrev1:
    ### TODO: YOUR CODE HERE ###
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)
    sw s2, 12(sp)
    li s0, 31
    add s1, x0, a0
    addi a0, x0, 0
    jal loop
loop:
    beq s0, x0, end
    andi s2, s1, 1
    sll s2, s2, s0 
    add a0, a0, s2
    srli s1, s1, 1
    addi s0, s0, -1
    jal loop
end:
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    lw s2, 12(sp)
    addi sp, sp, 16
    ret

# === The second version ===
# Reverse the bits in a0. Only use li, and, or, slli, srli!
bitrev2:
    ### TODO: YOUR CODE HERE ###
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)
    li s0, 0b1111111111111111
    and s1, a0, s0
    slli s0, s0, 16
    and a1, a0, s0
    slli s1, s1, 16
    srli a0, a0, 16
    or a0, a0, s1
    li s0, 0b111111110000000011111111
    and s1, a0, s0
    slli s0, s0, 8
    and a0, a0, s0
    slli s1, s1, 8
    srli a0, a0, 8
    or a0, a0, s1
    li s0, 0b1111000011110000111100001111
    and s1, a0, s0
    slli s0, s0, 4
    and a0, a0, s0
    slli s1, s1, 4
    srli a0, a0, 4
    or a0, a0, s1
    li s0, 0b110011001100110011001100110011
    and s1, a0, s0
    slli s0, s0, 2
    and a0, a0, s0
    slli s1, s1, 2
    srli a0, a0, 2
    or a0, a0, s1
    li s0, 0b1010101010101010101010101010101
    and s1, a0, s0
    slli s0, s0, 1
    and a0, a0, s0
    slli s1, s1, 1
    srli a0, a0, 1
    or a0, a0, s1
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 8
    ret
