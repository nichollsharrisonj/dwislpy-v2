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
	sw $ra,-72($sp)
	sw $fp,-76($sp)
	move $fp, $sp
	addi $sp,$sp,-96
	li $t0,1
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
	li $t0,3
	sw $t0,-8($fp)
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-24($fp)
	lw $a0,-24($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-28($fp)
	li $v0,4
	lw $a0,-28($fp)
	syscall
	lw $t1,-8($fp)
	move $t0,$t1
	sw $t0,-32($fp)
	lw $a0,-32($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-36($fp)
	li $v0,4
	lw $a0,-36($fp)
	syscall
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-40($fp)
	lw $t1,-8($fp)
	move $t0,$t1
	sw $t0,-44($fp)
	lw $t1,-40($fp)
	lw $t2,-44($fp)
	add $t0,$t1,$t2
	sw $t0,-12($fp)
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-52($fp)
	lw $t1,-8($fp)
	move $t0,$t1
	sw $t0,-56($fp)
	lw $t1,-52($fp)
	lw $t2,-56($fp)
	add $t0,$t1,$t2
	sw $t0,-48($fp)
	lw $a0,-48($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-60($fp)
	li $v0,4
	lw $a0,-60($fp)
	syscall
	lw $t1,-12($fp)
	move $t0,$t1
	sw $t0,-64($fp)
	lw $a0,-64($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-68($fp)
	li $v0,4
	lw $a0,-68($fp)
	syscall
main_done:
	lw $ra,-72($fp)
	lw $fp,-76($fp)
	addi $sp,$sp,96
	jr $ra
