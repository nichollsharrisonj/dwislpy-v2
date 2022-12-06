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
	sw $ra,-36($sp)
	sw $fp,-40($sp)
	move $fp, $sp
	addi $sp,$sp,-56
	li $t0,1
	sw $t0,-12($fp)
	li $t0,2
	sw $t0,-16($fp)
	lw $t1,-12($fp)
	lw $t2,-16($fp)
	sub $t0,$t1,$t2
	sw $t0,-4($fp)
	li $t0,3
	sw $t0,-8($fp)
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-24($fp)
	lw $t1,-8($fp)
	move $t0,$t1
	sw $t0,-28($fp)
	lw $t1,-24($fp)
	lw $t2,-28($fp)
	mult $t1,$t2
	mflo $t0
	sw $t0,-20($fp)
	lw $a0,-20($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-32($fp)
	li $v0,4
	lw $a0,-32($fp)
	syscall
main_done:
	lw $ra,-36($fp)
	lw $fp,-40($fp)
	addi $sp,$sp,56
	jr $ra
