        .text
        .globl main
main:   
        la $t0, msg
        lw $a0, 0($t0)
        li $v0, 4
        syscall

        la $a0, msg_data
        li $v0, 4
        syscall
        li $v0, 10
        syscall
        .data
        
msg_data:  .asciiz "Helloworld"
msg:       .word  msg_data
