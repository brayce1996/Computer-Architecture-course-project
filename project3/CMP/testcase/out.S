bne		$t0	$0	62
lw		$t0	$0	0
lh		$t1	$0	4
lhu		$t2	$0	6
lb		$t3	$0	3
lb		$t4	$0	5
lbu		$t5	$0	5
addi		$t0	$0	1
addi		$t1	$0	4
add		$s0	$t0	$t1
sll		$s0	$s0	2
lw		$t2	$s0	20
addi		$t4	$0	-40
sub		$s0	$s0	$t4
lw		$t3	$s0	-16
addi		$t0	$0	32
sll		$t1	$t0	2
sh		$t0	$t1	0
addi		$t0	$t0	-1
slti		$t2	$t0	24
beq		$t2	$0	-5
lw		$t0	$0	120
lw		$t1	$0	0
and		$t2	$t0	$t1
or		$t3	$t0	$t1
xor		$t4	$t0	$t1
nor		$t5	$t0	$t1
nand		$t6	$t0	$t1
slt		$t7	$t0	$t1
sll		$t1	$t1	3
srl		$t2	$t1	8
sra		$t3	$t1	8
srl		$t4	$t1	24
sra		$t5	$t1	24
srl		$t6	$t1	8
sra		$t7	$t1	8
sw		$t2	$0	120
sb		$t0	$0	121
sb		$t1	$0	123
lw		$t2	$0	120
lui		$t2	17185
andi		$t1	$t0	-3084
ori		$t2	$t0	772
nori		$t3	$t0	772
addi		$a0	$0	6
addi		$sp	$0	1024
jal		108
lw		$t0	$0	8
lw		$t1	$0	48
lw		$t2	$0	52
add		$t3	$t0	$t1
add		$t3	$t0	$t2
sub		$t3	$t0	$t1
sub		$t3	$t0	$t2
add		$t3	$t1	$t2
add		$t3	$t2	$t1
sub		$t3	$t1	$t2
sub		$t3	$t2	$t1
addi		$s1	$0	11
lw		$t0	$s1	1
addi		$s1	$0	43
lw		$s2	$0	48
j		4
lw		$t0	$0	0
lh		$t0	$0	1000
lhu		$t0	$0	400
lb		$t0	$0	600
lbu		$t0	$0	608
sw		$t0	$0	548
sh		$t0	$0	922
sb		$t0	$0	132
lw		$t0	$0	128
lh		$t0	$0	124
lhu		$t0	$0	120
lb		$t0	$0	116
lbu		$t0	$0	112
sw		$t0	$0	108
sh		$t0	$0	104
sb		$t0	$0	100
lw		$t0	$0	772
lh		$t0	$0	820
lhu		$t0	$0	348
lb		$t0	$0	652
lbu		$t0	$0	32
sw		$t0	$0	88
sh		$t0	$0	1008
sb		$t0	$0	804
lw		$t0	$0	696
lh		$t0	$0	8
lhu		$t0	$0	0
lb		$t0	$0	720
lbu		$t0	$0	440
sw		$t0	$0	276
sh		$t0	$0	436
sb		$t0	$0	128
halt
halt
halt
halt
halt
halt
halt
halt
addi		$sp	$sp	-8
sw		$ra	$sp	4
sw		$a0	$sp	0
bne		$a0	$0	5
lw		$a0	$sp	0
lw		$ra	$sp	4
addi		$sp	$sp	8
addi		$v0	$0	0
jr		$ra
slti		$t0	$a0	3
beq		$t0	$0	5
lw		$a0	$sp	0
lw		$ra	$sp	4
addi		$sp	$sp	8
addi		$v0	$0	1
jr		$ra
addi		$a0	$a0	-1
jal		108
add		$t0	$v0	$0
addi		$a0	$a0	-1
jal		108
add		$t1	$v0	$0
lw		$a0	$sp	0
lw		$ra	$sp	4
addi		$sp	$sp	8
add		$v0	$t0	$t1
jr		$ra
