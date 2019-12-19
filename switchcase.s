        .text
        .globl main

main:
        li $v0, 5
        syscall
        move $s0, $a0
        add $s0, $s0, $s0
        add $s0, $s0, $s0  #s0 = s0 * 4
        la $t0, L
        add $t0, $t0, $s0
        lw $t0, 0($t0)
        jr $t0
L0:
        la $a0, ZERO
        li $v0, 4
        syscall
        j ENDCASE
L1:
        la $a0, ONE
        li $v0, 4
        syscall
        j ENDCASE
L2:
        la $a0, TWO
        li $v0, 4
        syscall
        j ENDCASE
        
ENDCASE:        
        li $v0, 10
        syscall
        
        .data
L:  .word  L0 L1 L2
ZERO: .asciiz "Case zero\n"
ONE: .asciiz "Case one\n" 
TWO:  .asciiz "Case two\n"
