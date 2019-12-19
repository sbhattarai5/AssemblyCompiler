        .text
        .globl main

main:
        li $t0, 1
        li $t0, 2
        add $a0, $t0, $t0
        li $v0, 1
        syscall
        li $v0, 10
        syscall
        
