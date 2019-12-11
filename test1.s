        .text
        .globl main
main:
        jal function
        beq $a0, $a1, function
        
function: li $v0, 1   ## test_comment
          add $v0, $v0, $v1
          j main      
         .data
board:
test2:  .asciiz "Hello world\n\""
check_function: .word 1 2 test2 function
