0
256
lw      $t0 $0  0
lw      $t1 $0  4
lw      $t2 $0  8
lw      $t3 $0  12
lw      $t4 $0  16
lw      $t5 $0  20
lw      $t6 $0  24
lw      $t7 $0  28
lw      $t8 $0  32
lw      $t9 $0  36

add     $s0 $t5 $t2
add     $s0 $t2 $t4
sub     $s0 $t5 $t2
sub     $s0 $t6 $t6
sub     $s0 $t8 $t9
sub     $s0 $t2 $t2
sub     $s0 $0  $t6
addi    $s0 $t5 -32768
addi    $s0 $t6 32768

sll     $0  $0  0
sll     $0  $t0 0
sll     $0  $t1 0
sll     $0  $0  1
sll     $0  $t0 1
sll     $0  $t1 1

add     $0  $0  $0
add     $s0 $t6 $t7
add     $s0 $t8 $t9

addu    $0  $0  $0
addu    $s0 $t6 $t6
addu    $s0 $s0 $t6

sub     $0  $0  $0
sub     $s0 $t9 $t5

and     $0  $0  $0
and     $s0 $t2 $t8

or      $0  $0  $0
or      $s0 $t6 $t5

xor     $0  $0  $0
xor     $s0 $t2 $t6

nor     $0  $0  $0
nor     $s0 $t5 $t1

nand    $0  $0  $0
nand    $s0 $t2 $t5

slt     $0  $0  $0
slt     $s0 $t6 $t7
slt     $s0 $t5 $t6

sll     $s0 $t1 31
sll     $s0 $t2 31
sll     $s0 $t5 -1

srl     $0  $0  0
srl     $s0 $t6 20
srl     $s0 $t1 -1

sra     $0  $0  0
sra     $s0 $t6 10
sra     $s0 $t6 31
sra     $s0 $t5 20

addi    $0  $t5 1
addi    $s0 $t6 -1
addi    $s0 $s1 32638

addiu   $0  $t5 1
addiu   $s0 $t6 -1
addiu   $s0 $t6 -32638
addiu   $s0 $t5 32638

lw      $0  $0  0
addi    $sp $0  0
lw      $s0 $sp 4
addi    $sp $0  4
lw      $s0 $sp -4

lh      $0  $0  0
lh      $s0 $0  24
lh      $s0 $0  26
lh      $s0 $0  28
lh      $s0 $0  30

lhu     $0  $0  0
lhu     $s0 $0  20
lhu     $s0 $0  22
lhu     $s0 $0  24
lhu     $s0 $0  26

lb      $0  $0  0
lb      $s0 $0  20
lb      $s0 $0  21
lb      $s0 $0  22
lb      $s0 $0  23
lb      $s0 $0  24
lb      $s0 $0  25

lbu     $0  $0  0
lbu     $s0 $0  20
lbu     $s0 $0  21
lbu     $s0 $0  22
lbu     $s0 $0  23
lbu     $s0 $0  24
lbu     $s0 $0  25

sw      $0  $0  256
addi    $sp $0  256
sw      $t5 $sp 0
lw      $s0 $sp 0
sw      $t6 $sp 4
lw      $s0 $sp 4

sh      $0  $0  256
sh      $t4 $sp 0
lw      $s0 $sp 0
sh      $t5 $sp 2
lw      $s0 $sp 0
sh      $t5 $sp 4
lw      $s0 $sp 4
sh      $t6 $sp 6
lw      $s0 $sp 4

sb      $0  $0  256
sb      $t5 $sp 0
sb      $t5 $sp 1
sb      $t5 $sp 2
sb      $t5 $sp 3
lw      $s0 $sp 0

lui     $0  1
lui     $s0 -32768
lui     $s0 -1

andi    $0  $0  1
andi    $s0 $t8 -1

ori     $0  $0  1
lw      $s0 $0  40
ori     $s0 $s0 -1

nori    $0  $0  -1
nori    $s0 $t5 1
nori    $s0 $t0 -1

slti    $0  $0  1
slti    $s0 $t3 -32768
slti    $s0 $t6 -32768
slti    $s0 $t1 32768

beq     $t6 $t7 1
lw      $ra $0  20

bne     $t6 $t7 1
lw      $ra $0  24

lw      $s0 $0  40
bgtz    $s0 1
lw      $ra $0  28
bgtz    $0  1
lw      $ra $0  32

j   254
jal 253

lw      $sp $0  24
lw      $0  $sp 32769
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
halt
jal 139
j   138
halt