        .text
        .globl main
main:
        li $v0, 5  #input
        syscall
        move $s0, $a0 #s0 = first_input()
        syscall
        move $s1, $a0 #s1 = second_input()
        add $a0, $s0, $s1 #a0 = s0 + s1
        li $v0, 1
        syscall
        li $v0, 10
        syscall
        
 
        .data
board:
test2:  .asciiz "Hello world\n\""
check_function: .word -1 2 test2 function
