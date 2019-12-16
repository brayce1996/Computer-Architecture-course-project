0
50
addi    $at $0  -1
lw      $t0 $0  8
lw      $t1 $0  4
add     $t2 $t1 $t0
addi    $t0 $t0 -32768
lw      $s0 $0  8
lw      $s1 $0  12
add     $s2 $s1 $s0
lw      $s0 $0  0
lw      $s1 $0  8
sub     $s2 $s0 $s1
lw      $t1 $0  16
addi    $t1 $t1 32768
lw      $0  $0  4
lw      $t0 $0  16
lw      $t1 $0  16
sub     $t2 $t1 $t0
lw      $t0 $0  8
lw      $t1 $0  8
sub     $t2 $t1 $t0
lw      $t0 $0  20
lw      $t1 $0  24
sub     $t2 $t1 $t0
lw      $t1 $0  16
sub     $t2 $0  $t1
sll     $0  $0  0
lw      $t1 $0  28
sll     $0  $t1 0
lw      $t1 $0  32
sll     $0  $t1 0
sll     $0  $0  1
lw      $t1 $0  28
sll     $0  $t1 0
lw      $sp $0  36
lb      $t0 $sp 0
lh      $t0 $sp 0
lw      $t0 $sp 0
lhu     $t0 $0  0
lhu     $t0 $0  16
lbu     $t0 $0  0
lbu     $t0 $0  16
addi    $sp $0  256
sw      $t0 $sp 0
sh      $t0 $sp 4
sb      $t0 $sp 8
lw      $sp $0  16
lw      $0  $sp 32769
halt
halt
halt
halt
halt
halt