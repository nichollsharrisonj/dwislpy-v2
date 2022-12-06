	.data
L_15:
	.asciiz "False"
L_14:
	.asciiz "True"
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
	sw $ra,-64($sp)
	sw $fp,-68($sp)
	move $fp, $sp
	addi $sp,$sp,-88
	li $t0,1
	sw $t0,-4($fp)
	lw $a0,-4($fp)
	jal p
	li $t0,1
	sw $t0,-12($fp)
	li $t0,3
	sw $t0,-16($fp)
	li $t0,5
	sw $t0,-20($fp)
	li $t0,1
	sw $t0,-28($fp)
	li $t0,2
	sw $t0,-32($fp)
	lw $t1,-28($fp)
	lw $t2,-32($fp)
	sub $t0,$t1,$t2
	sw $t0,-24($fp)
	lw $a3,-24($fp)
	lw $a2,-20($fp)
	lw $a1,-16($fp)
	lw $a0,-12($fp)
	jal f
	move $t0,$v0
	sw $t0,-8($fp)
	lw $a0,-8($fp)
	li $v0,1
	syscall
	la $t0,L_0
	sw $t0,-36($fp)
	li $v0,4
	lw $a0,-36($fp)
	syscall
	li $t0,1
	sw $t0,-44($fp)
	lw $a0,-44($fp)
	jal cond
	move $t0,$v0
	sw $t0,-40($fp)
	lw $t1,-40($fp)
	beqz $t1,L_12
	j L_11
L_11:
	la $t0,L_14
	sw $t0,-48($fp)
	li $v0,4
	lw $a0,-48($fp)
	syscall
	la $t0,L_0
	sw $t0,-52($fp)
	li $v0,4
	lw $a0,-52($fp)
	syscall
	j L_13
L_12:
	la $t0,L_15
	sw $t0,-56($fp)
	li $v0,4
	lw $a0,-56($fp)
	syscall
	la $t0,L_0
	sw $t0,-60($fp)
	li $v0,4
	lw $a0,-60($fp)
	syscall
L_13:
main_done:
	lw $ra,-64($fp)
	lw $fp,-68($fp)
	addi $sp,$sp,88
	jr $ra
cond:
	sw $ra,-8($sp)
	sw $fp,-12($sp)
	move $fp, $sp
	addi $sp,$sp,-32
	sw $a0,0($fp)
	lw $t1,0($fp)
	bgtz $t1,L_5
	j L_6
L_5:
	li $t0,1
	sw $t0,-4($fp)
	j L_7
L_6:
	li $t0,0
	sw $t0,-4($fp)
L_7:
	lw $v0,-4($fp)
	j cond_done
cond_done:
	lw $ra,-8($fp)
	lw $fp,-12($fp)
	addi $sp,$sp,32
	jr $ra
f:
	sw $ra,-32($sp)
	sw $fp,-36($sp)
	move $fp, $sp
	addi $sp,$sp,-56
	sw $a0,0($fp)
	sw $a1,4($fp)
	sw $a2,8($fp)
	sw $a3,12($fp)
	lw $t1,0($fp)
	move $t0,$t1
	sw $t0,-24($fp)
	lw $t1,4($fp)
	move $t0,$t1
	sw $t0,-28($fp)
	lw $t1,-24($fp)
	lw $t2,-28($fp)
	add $t0,$t1,$t2
	sw $t0,-16($fp)
	lw $t1,8($fp)
	move $t0,$t1
	sw $t0,-20($fp)
	lw $t1,-16($fp)
	lw $t2,-20($fp)
	add $t0,$t1,$t2
	sw $t0,-8($fp)
	lw $t1,12($fp)
	move $t0,$t1
	sw $t0,-12($fp)
	lw $t1,-8($fp)
	lw $t2,-12($fp)
	mult $t1,$t2
	mflo $t0
	sw $t0,-4($fp)
	lw $v0,-4($fp)
	j f_done
f_done:
	lw $ra,-32($fp)
	lw $fp,-36($fp)
	addi $sp,$sp,56
	jr $ra
p:
	sw $ra,-16($sp)
	sw $fp,-20($sp)
	move $fp, $sp
	addi $sp,$sp,-40
	sw $a0,0($fp)
	lw $t1,0($fp)
	bgtz $t1,L_8
	j L_9
L_8:
	la $t0,L_1
	sw $t0,-4($fp)
	j L_10
L_9:
	la $t0,L_2
	sw $t0,-4($fp)
L_10:
	li $v0,4
	lw $a0,-4($fp)
	syscall
	la $t0,L_0
	sw $t0,-8($fp)
	li $v0,4
	lw $a0,-8($fp)
	syscall
	li $t0,0
	sw $t0,-12($fp)
	lw $v0,-12($fp)
	j p_done
p_done:
	lw $ra,-16($fp)
	lw $fp,-20($fp)
	addi $sp,$sp,40
	jr $ra
