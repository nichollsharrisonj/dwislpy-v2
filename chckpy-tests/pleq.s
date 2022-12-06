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
	sw $ra,-24($sp)
	sw $fp,-28($sp)
	move $fp, $sp
	addi $sp,$sp,-48
	li $t0,1
	sw $t0,-4($fp)
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-8($fp)
	li $t0,1
	sw $t0,-12($fp)
	lw $t1,-8($fp)
	lw $t2,-12($fp)
	add $t0,$t1,$t2
	sw $t0,-4($fp)
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-16($fp)
	lw $a0,-16($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-20($fp)
	li $v0,4
	lw $a0,-20($fp)
	syscall
main_done:
	lw $ra,-24($fp)
	lw $fp,-28($fp)
	addi $sp,$sp,48
	jr $ra
