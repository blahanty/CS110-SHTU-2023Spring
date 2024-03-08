.section .data
circle: .string "circle"
square: .string "square"
.section .text
.globl draw
.type draw, @function
draw:
	addi sp, sp, -20
	sw a0, 0(sp)
	sw a1, 4(sp)
	sw a2, 8(sp)
	sw a3, 12(sp)
	sw a4, 16(sp)
	li a0, 20
	li a1, 20
	li a2, 10
	li a3, 0xFFFF
	jal LCD_DrawCircle
	li a0, 5
	li a1, 50
	la a2, circle
	li a3, 0xFFFF
	jal LCD_ShowString
	li a0, 75
	li a1, 5
	li a2, 100
	li a3, 30
	li a4, 0xFFFF
	jal LCD_DrawRectangle
	li a0, 80
	li a1, 50
	la a2, square
	li a3, 0xFFFF
	jal LCD_ShowString
	lw a0, 0(sp)
	lw a1, 4(sp)
	lw a2, 8(sp)
	lw a3, 12(sp)
	lw a4, 16(sp)
	addi sp, sp, 20

