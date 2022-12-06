	.data
L_4:
	.asciiz "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
L_2:
	.asciiz "False"
L_1:
	.asciiz "True"
L_3:
	.asciiz "None"
L_0:
	.asciiz "\n"
	.text
	.globl main
main:
	sw $ra,-8($sp)
	sw $fp,-12($sp)
	move $fp, $sp
	addi $sp,$sp,-32
	li $t0,0
	sw $t0,-4($fp)
main_done:
	lw $ra,-8($fp)
	lw $fp,-12($fp)
	addi $sp,$sp,32
	jr $ra
