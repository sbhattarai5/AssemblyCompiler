        .text
        .globl main
add:
        li $v0, 5
        syscall
        move $s0, $a0
        syscall
        add $a0, $a0, $s0
        li $v0, 1
        syscall
        jr $ra

main:
        jal add
        li $v0, 10
        syscall

        .data
        
NEWLINE: .asciiz   "\n"
        
