	.data
L_15:
	.asciiz "Done."
L_14:
	.asciiz "no"
L_9:
	.asciiz "no"
L_8:
	.asciiz "yes"
L_4:
	.asciiz "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
L_2:
	.asciiz "False"
L_1:
	.asciiz "True"
L_13:
	.asciiz "yes"
L_3:
	.asciiz "None"
L_0:
	.asciiz "\n"
	.text
	.globl main
main:
	sw $ra,-48($sp)
	sw $fp,-52($sp)
	move $fp, $sp
	addi $sp,$sp,-72
	li $t0,1
	sw $t0,-4($fp)
	lw $t1,-4($fp)
	bgtz $t1,L_5
	j L_6
L_5:
	la $t0,L_8
	sw $t0,-8($fp)
	li $v0,4
	lw $a0,-8($fp)
	syscall
	la $t0,L_0
	sw $t0,-12($fp)
	li $v0,4
	lw $a0,-12($fp)
	syscall
	j L_7
L_6:
	la $t0,L_9
	sw $t0,-16($fp)
	li $v0,4
	lw $a0,-16($fp)
	syscall
	la $t0,L_0
	sw $t0,-20($fp)
	li $v0,4
	lw $a0,-20($fp)
	syscall
L_7:
	li $t0,0
	sw $t0,-4($fp)
	lw $t1,-4($fp)
	bgtz $t1,L_10
	j L_11
L_10:
	la $t0,L_13
	sw $t0,-24($fp)
	li $v0,4
	lw $a0,-24($fp)
	syscall
	la $t0,L_0
	sw $t0,-28($fp)
	li $v0,4
	lw $a0,-28($fp)
	syscall
	j L_12
L_11:
	la $t0,L_14
	sw $t0,-32($fp)
	li $v0,4
	lw $a0,-32($fp)
	syscall
	la $t0,L_0
	sw $t0,-36($fp)
	li $v0,4
	lw $a0,-36($fp)
	syscall
L_12:
	la $t0,L_15
	sw $t0,-40($fp)
	li $v0,4
	lw $a0,-40($fp)
	syscall
	la $t0,L_0
	sw $t0,-44($fp)
	li $v0,4
	lw $a0,-44($fp)
	syscall
main_done:
	lw $ra,-48($fp)
	lw $fp,-52($fp)
	addi $sp,$sp,72
	jr $ra
