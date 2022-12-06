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
	sw $ra,-68($sp)
	sw $fp,-72($sp)
	move $fp, $sp
	addi $sp,$sp,-88
	j L_8
L_8:
	j L_5
L_5:
	la $t0,L_1
	sw $t0,-4($fp)
	j L_7
L_6:
	la $t0,L_2
	sw $t0,-4($fp)
L_7:
	li $v0,4
	lw $a0,-4($fp)
	syscall
	la $t0,L_0
	sw $t0,-8($fp)
	li $v0,4
	lw $a0,-8($fp)
	syscall
	j L_12
L_12:
	j L_10
L_9:
	la $t0,L_1
	sw $t0,-12($fp)
	j L_11
L_10:
	la $t0,L_2
	sw $t0,-12($fp)
L_11:
	li $v0,4
	lw $a0,-12($fp)
	syscall
	la $t0,L_0
	sw $t0,-16($fp)
	li $v0,4
	lw $a0,-16($fp)
	syscall
	j L_16
L_16:
	j L_13
L_13:
	la $t0,L_1
	sw $t0,-20($fp)
	j L_15
L_14:
	la $t0,L_2
	sw $t0,-20($fp)
L_15:
	li $v0,4
	lw $a0,-20($fp)
	syscall
	la $t0,L_0
	sw $t0,-24($fp)
	li $v0,4
	lw $a0,-24($fp)
	syscall
	j L_20
L_20:
	j L_18
L_17:
	la $t0,L_1
	sw $t0,-28($fp)
	j L_19
L_18:
	la $t0,L_2
	sw $t0,-28($fp)
L_19:
	li $v0,4
	lw $a0,-28($fp)
	syscall
	la $t0,L_0
	sw $t0,-32($fp)
	li $v0,4
	lw $a0,-32($fp)
	syscall
	li $t0,1
	sw $t0,-40($fp)
	li $t0,2
	sw $t0,-44($fp)
	lw $t1,-40($fp)
	lw $t2,-44($fp)
	blt $t1,$t2,L_21
	j L_22
L_21:
	la $t0,L_1
	sw $t0,-36($fp)
	j L_23
L_22:
	la $t0,L_2
	sw $t0,-36($fp)
L_23:
	li $v0,4
	lw $a0,-36($fp)
	syscall
	la $t0,L_0
	sw $t0,-48($fp)
	li $v0,4
	lw $a0,-48($fp)
	syscall
	li $t0,1
	sw $t0,-56($fp)
	li $t0,0
	sw $t0,-60($fp)
	lw $t1,-56($fp)
	lw $t2,-60($fp)
	beq $t1,$t2,L_24
	j L_25
L_24:
	la $t0,L_1
	sw $t0,-52($fp)
	j L_26
L_25:
	la $t0,L_2
	sw $t0,-52($fp)
L_26:
	li $v0,4
	lw $a0,-52($fp)
	syscall
	la $t0,L_0
	sw $t0,-64($fp)
	li $v0,4
	lw $a0,-64($fp)
	syscall
main_done:
	lw $ra,-68($fp)
	lw $fp,-72($fp)
	addi $sp,$sp,88
	jr $ra
