	.data
L_8:
	.asciiz "Done."
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
	sw $ra,-40($sp)
	sw $fp,-44($sp)
	move $fp, $sp
	addi $sp,$sp,-64
	li $t0,0
	sw $t0,-4($fp)
L_5:
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-8($fp)
	li $t0,10
	sw $t0,-12($fp)
	lw $t1,-8($fp)
	lw $t2,-12($fp)
	blt $t1,$t2,L_6
	j L_7
L_6:
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
	lw $t1,-4($fp)
	move $t0,$t1
	sw $t0,-24($fp)
	li $t0,1
	sw $t0,-28($fp)
	lw $t1,-24($fp)
	lw $t2,-28($fp)
	add $t0,$t1,$t2
	sw $t0,-4($fp)
	j L_5
L_7:
	la $t0,L_8
	sw $t0,-32($fp)
	li $v0,4
	lw $a0,-32($fp)
	syscall
	la $t0,L_0
	sw $t0,-36($fp)
	li $v0,4
	lw $a0,-36($fp)
	syscall
main_done:
	lw $ra,-40($fp)
	lw $fp,-44($fp)
	addi $sp,$sp,64
	jr $ra
