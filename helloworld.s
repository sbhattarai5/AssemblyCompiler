        .text
        .globl main
main:
        la $a0, HELLOWORLD
        li $v0, 4
        syscall
        li $v0, 10
        syscall
        .data
HELLOWORLD:      .asciiz "Hello world\n"
